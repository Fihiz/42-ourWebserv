/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 11:59:24 by sad-aude          #+#    #+#             */
/*   Updated: 2021/07/23 15:37:45 by sad-aude         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "WebservData.hpp"
#include "../config/Parser.hpp"
#include "../config/Config.hpp"
#include "Respond.hpp"

Socket *findSpecificMasterSocket(std::vector<Socket *> tabMaster, int fd)
{
    std::vector<Socket *>::iterator ite = tabMaster.end();
    for (std::vector<Socket *>::iterator it = tabMaster.begin(); it != ite; it++)
    {
        if ((*it)->getMasterSock() == fd)
            return(*it);
    }
    return (NULL);
}

void    processMasterSocket(WebservData &Data, int fd)
{
    int     clientSock;

    clientSock = accept(fd, NULL, NULL);
    if (clientSock < 0)
        error("Accept connexion", Data);
    FD_SET(clientSock, &Data.getReadSet());
    std::cout << T_BB "New connexion established on [" T_GNB << clientSock << T_BB "]\n" T_N << std::endl;
    return ;
}

Config *findConfigForClient(WebservData &Data, std::string host)
{
    if (host.find(":") == std::string::npos)
    {
       host = host + ":80"; 
    }
    if (Data.getMapServerName().find(host) != Data.getMapServerName().end())
        return (&(Data.getMapServerName()[host]));
    return (NULL);
}

Respond    setFullPathInfo(const t_location *locationForClient, t_request &parsedRequest, Config &configForClient, std::string &tmpFile)
{
    if (locationForClient)
    {
        parsedRequest.fullPathInfo = parsedRequest.pathInfo;
        parsedRequest.fullPathInfo.replace(0, tmpFile.size() + 1, configForClient.getRoot(tmpFile));
        return (checkingHeader(&parsedRequest, locationForClient->method, configForClient));
    }
    else
    {
        std::vector<std::string>    method;
        method.push_back("GET");
        parsedRequest.fullPathInfo = configForClient.getRoot("") + parsedRequest.pathInfo.substr(2);
        return (checkingHeader(&parsedRequest, method, configForClient));
    }
}

const t_location *findLocationForClient(Config &configForClient, t_request &parsedRequest, Respond &clientRespond)
{
    const t_location  *locationForClient = NULL;
    std::string tmpFile = parsedRequest.pathInfo.substr(1);

    while(tmpFile.size() > 0 && locationForClient == NULL)
    {
        locationForClient = configForClient.getLocation(tmpFile);
        if (!locationForClient)
            tmpFile.resize(tmpFile.size() - 1);
    }
    clientRespond = setFullPathInfo(locationForClient, parsedRequest, configForClient, tmpFile);

    return (locationForClient);
}
    
void	redirectCgiOutputToClient(char **env, int fd, t_request req, Respond &resp, WebservData &Data, const t_location  *locationForClient, Config *configForClient)
{
	int p[2], pid;
	
	pipe(p);
	char *argv2[2];
	argv2[0] = (char *)"../prog/php-cgi";
	argv2[1] = (char *)"../images/file.php";
	std::string str, str1;
	str1.resize(1024);
	
	pid = fork();
	if (pid == 0)
	{
		close(p[0]);
		dup2(p[1], 1);
		if (execve(req.pathInfoCgi.c_str(), argv2, env) == -1)
		{
			resp.setStatusCode("500 Internal Error");
			resp.finalRespond(fd, req, env, Data, locationForClient, configForClient );
			std::cerr << "Internal Error\n";
			close(p[1]);
			exit(0);
		}
		close(p[1]);
		exit (0);
	}
	else
	{
		waitpid(-1, NULL, 0);
		close(p[1]);
		dup2(p[0], fd);
		while(read(p[0], &str1[0], 1024) == 1024)
		{
			str  = str + str1;
			str1.clear();
			str1.resize(1024);
		}
		str  = str + str1;
		send(p[0], str.c_str(), str.length(), 0);
	}
}

void    checkRedir(Config *configForClient, t_request &parsedRequest, Respond &clientRespond)
{
    if (parsedRequest.pathInfo[parsedRequest.pathInfo.size() - 1] != '/')
    {
        struct stat statBuf;
        int ret = stat((parsedRequest.pathInfo).c_str(), &statBuf);
        if ((ret != -1 && S_ISDIR(statBuf.st_mode)) || (configForClient->getLocation(parsedRequest.pathInfo.substr(1) + "/")))
        {
            //parsedRequest.statusCode = "301 Moved Permanently";
            clientRespond.setStatusCode("301 Moved Permanently");
            parsedRequest.location = parsedRequest.pathInfo.substr(1) + "/";
        }
    }
    (void) configForClient;
}

/* TEST BOUCLE RECV WHILE AVEC TAILLE DE 1 */
int     processSockets(int fd, WebservData &Data, char **env)
{
    char    requestBuffer[1];
    int     running = 1;
    std::string tmpRequest;

    struct stat buf;
    fstat(fd, &buf);
    off_t fdSize = buf.st_size;
    std::cout << T_BB "SIZE OF FD = " << fdSize << T_N << std::endl;

	Respond	clientRespond;

    if (isTabMaster(Data.getTabMaster(), fd) == 1)
        processMasterSocket(Data, fd);
    else
    {
        ssize_t len = 1;
        while (len > 0 && tmpRequest.size() < (size_t)fdSize)
        {
            len = recv(fd, requestBuffer, 1, 0);

            std::cout << " \r \r \r";
            tmpRequest =  tmpRequest  + requestBuffer[0];
        }
        if (len < 0)
            losingConnexion(fd, Data.getReadSet(), "Connexion lost... (");
        else
        {
            tmpRequest =  tmpRequest + '\0';
			t_request	parsedRequest;



            // ----------------------------------------------
            (void) env; //

			parsedRequest = parsingRequest(tmpRequest);
            //parsedRequest.statusCode = "200 OK";
            //clientRespond.setStatusCode("400 Bad Request");

            Config *configForClient;

            configForClient = findConfigForClient(Data, parsedRequest.host);
            const t_location  *locationForClient = NULL;
            if (!configForClient)
            {
                //parsedRequest.statusCode = "400 Bad Request"; //   
                clientRespond.setStatusCode("400 Bad Request");
            }
            else
            {
                checkRedir(configForClient, parsedRequest, clientRespond);
                if (clientRespond.getStatusCode() == 200 && clientRespond.getStatusMessage() == " OK")
                {
                    locationForClient = findLocationForClient(*configForClient, parsedRequest, clientRespond);      
                }
            }
			std::cout << T_CB << "[" T_GNB << fd << T_CB "]" << " is requesting :" << T_N  << std::endl << tmpRequest << std::endl;
            running = clientRespond.finalRespond(fd, parsedRequest, env, Data, locationForClient, configForClient);
			// std::cout << "RUNNING: " << running << "\n";
        }
    }
    return (running);
}

std::vector<Config> configuration(int argc, char **argv) {
	std::string path;
	if (argc != 2)
    {
        std::cout << "In view of arguments, the default configuration is used." << std::endl;
		path = "config/default";
    }
	else
		path = argv[1];
	Parser	file(path);
	file.setContentFile();
	file.checkGeneralSyntax();
	file.setConfiguration();

    file.printConfigs();

	return (file.getConfiguration());
}

int     main(int ac, char *av[], char *env[])
{
    (void)env;
	std::vector<Config> setup;
	try {
		setup = configuration(ac, av);
	}
	catch (std::exception & err) {
		std::cout << err.what() << std::endl;
		return (1);
	}

    WebservData Data(setup);
    
    int running = 1;
    std::cout << std::endl;

    while (running)
    {
        std::cout << T_GYB "Waiting in passive mode" T_N << std::endl;
        Data.getReadCopy() = Data.getReadSet();
        if (select(FD_SETSIZE, &Data.getReadCopy(), 0, 0, 0) < 0)
            error("Select", Data);
        for (int fd = 0; fd <= FD_SETSIZE; ++fd)
        {
            if (FD_ISSET(fd, &Data.getReadCopy()))
            {
                running = processSockets(fd, Data, env);
                break ;
            }
        }
    }
    closeAllFdUnlessMaster(Data.getReadSet(), Data.getTabMaster()); // Destructor destroys the master
    destroyTabMaster(Data.getTabMaster());
    return (0);
}