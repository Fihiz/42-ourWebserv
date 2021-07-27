/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 11:59:24 by sad-aude          #+#    #+#             */
/*   Updated: 2021/07/27 15:28:56 by sad-aude         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "WebservData.hpp"
#include "../config/Parser.hpp"
#include "../config/Config.hpp"

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

void    setFullPathInfo(t_request &parsedRequest, Config &configForClient, std::string &tmpFile)
{
    parsedRequest.fullPathInfo = parsedRequest.pathInfo;
    parsedRequest.fullPathInfo.replace(0, tmpFile.size(), configForClient.getRoot(tmpFile));
}

const t_location *findLocationForClient(Config &configForClient, t_request &parsedRequest)
{
    const t_location  *locationForClient = NULL;
    std::string tmpFile = parsedRequest.pathInfo;

    while(tmpFile.size() > 0 && locationForClient == NULL)
    {
        locationForClient = configForClient.getLocation(tmpFile);
        if (!locationForClient)
            tmpFile.resize(tmpFile.size() - 1);
    }
    setFullPathInfo(parsedRequest, configForClient, tmpFile);
    checkingMethod(parsedRequest, locationForClient->method);

    return (locationForClient);
}

void    checkRedir(Config *configForClient, t_request &parsedRequest)
{
    if (parsedRequest.pathInfo[parsedRequest.pathInfo.size() - 1] != '/')
    {
        struct stat statBuf;
        int ret = stat((parsedRequest.pathInfo).c_str(), &statBuf);
        if ((ret != -1 && S_ISDIR(statBuf.st_mode)) || (configForClient->getLocation(parsedRequest.pathInfo + "/")))
        {
            parsedRequest.statusCode = "301 Moved Permanently";
            parsedRequest.location = parsedRequest.pathInfo + "/";
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


    if (isTabMaster(Data.getTabMaster(), fd) == 1)
        processMasterSocket(Data, fd);
    else
    {
        ssize_t len = 1;
        while ((tmpRequest.size() < (size_t)fdSize) && ((len = recv(fd, requestBuffer, 1, 0)) >= 0) )
        {
            std::cout << " \r \r \r";
            tmpRequest =  tmpRequest  + requestBuffer[0];
            if (len == 0)
                break;
        }
        if (len < 0)
            losingConnexion(fd, Data.getReadSet(), "Connexion lost... (");
        else
        {
            tmpRequest = tmpRequest + '\0';
			t_request	parsedRequest;
 
            (void) env;

			parsedRequest = parsingRequest(tmpRequest);
            Config *configForClient;
            const t_location  *locationForClient = NULL;

            if (parsedRequest.statusCode == "200 OK")
            {
                configForClient = findConfigForClient(Data, parsedRequest.host);
                if (!configForClient)
                    parsedRequest.statusCode = "400 Bad Request";
                else
                {
                    if ((unsigned long)atoi(parsedRequest.contentLenght.c_str()) > configForClient->getMaxBodySize())
                        parsedRequest.statusCode = "413 Request Entity Too Large"; // A changer ?
                    checkRedir(configForClient, parsedRequest);
                    if (parsedRequest.statusCode == "200 OK")
                    {
                        locationForClient = findLocationForClient(*configForClient, parsedRequest);
                        if ((parsedRequest.statusCode != "400 Bad Request")
                            && (locationForClient->redirect != ""))
                        {
                            parsedRequest.statusCode = "301 Moved Permanently";
                            parsedRequest.location = locationForClient->redirect;
                        }

                        std::cout << "PATH : " << parsedRequest.fullPathInfo << std::endl;        
                    }
                }
            }
            std::string responseToClient;
            if (parsedRequest.statusCode == "301 Moved Permanently")
            {
                responseToClient = "HTTP/1.1 301 Moved Permanently\nLocation: " + parsedRequest.location;
            }
            else
            {
                if (parsedRequest.statusCode == "200 OK")
                    setContentDependingOnFileOrDirectory(parsedRequest, locationForClient, configForClient);
                else
                    parsedRequest.fileContent = getContentFileError(configForClient, parsedRequest.statusCode);
                responseToClient = "HTTP/1.1 " +  parsedRequest.statusCode + "\nContent-Type:" + parsedRequest.fileType + "\nContent-Length:" 
                                        + std::to_string(parsedRequest.fileContent.size()) + "\n\n" + parsedRequest.fileContent;          
            }
            if (parsedRequest.pathInfo == "/exit.html") // (?)
                running = 0;
			std::cout << T_CB << "[" T_GNB << fd << T_CB "]" << " is requesting :" << T_N  << std::endl << tmpRequest << std::endl;
            // std::cout << "WE PRINT THE RESPONSE TO CLIENT HERE" << std::endl << T_YB << responseToClient.c_str() << T_N << "UNTIL HERE"<< std::endl;
            std::cout << T_GYB "Current status code [" T_GNB << parsedRequest.statusCode << T_GYB << "]" << T_N << std::endl;
            std::cout << " \r \r \r";
            fcntl(fd, F_SETFL, O_NONBLOCK);
            if (send(fd, responseToClient.c_str(), responseToClient.size(), 0) < 0)
                error("Send", Data);
            losingConnexion( fd, Data.getReadSet(), "Closing... [");
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

    /* TIMEOUT
    struct timeval  tv;
    tv.tv_sec = 120;
    tv.tv_usec = 0; */
    int ret;

    while (running)
    {
        std::cout << T_GYB "Waiting in passive mode" T_N << std::endl;
        Data.getReadCopy() = Data.getReadSet();
        ret = select(FD_SETSIZE, &Data.getReadCopy(), 0, 0, 0);
        if (ret < 0)
            error("Select", Data);
        else if (ret == 0)
            running = 0;
        else
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