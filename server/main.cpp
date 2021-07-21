/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgoudet <pgoudet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 11:59:24 by sad-aude          #+#    #+#             */
/*   Updated: 2021/07/21 16:24:39 by pgoudet          ###   ########.fr       */
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
    {
        std::cout << "Config has been found" << std::endl;
        return (&(Data.getMapServerName()[host]));
    }
    return (NULL);
}

void	redirectCgiOutputToClient(char **env, int fd, t_request req, Respond &resp, WebservData &Data)
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
			resp.finalRespond(fd, req, env, Data);
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

int     processSockets(int fd, WebservData &Data, char **env)
{
    (void) Data;
    char    requestBuffer[20000];
    int     running = 1;
	Respond	clientRespond;

    if (isTabMaster(Data.getTabMaster(), fd) == 1)
        processMasterSocket(Data, fd);
    else
    {
        ssize_t len = recv(fd, requestBuffer, 19999, 0);
        if (len < 0)
            losingConnexion(fd, Data.getReadSet(), "Connexion lost... (");
        else
        {
            requestBuffer[len] = '\0';
			t_request	parsedRequest;
 			Config *configForClient;
			
			parsedRequest = parsingRequest(requestBuffer);
			configForClient = findConfigForClient(Data, parsedRequest.host);
			std::cout << "res=" << parsedRequest.host << std::endl;
            if (!configForClient)
            {
				clientRespond.setStatusCode("400 Bad Request");
            }
            else
            {
                parsedRequest.fullPathInfo = configForClient->getRoot("") + parsedRequest.pathInfo.substr(2);
                std::cout << "PATH : " << parsedRequest.fullPathInfo << std::endl; 
                
                const t_location  *locationForClient;
                locationForClient = configForClient->getLocation("/images");
                if (locationForClient)
                    clientRespond = checkingHeader(&parsedRequest, locationForClient->method, *configForClient);
                std::cout << "PORT CONFIG : ";
                configForClient->printListen();
				std::cout << "HOST NAME : " << configForClient->getServerName() << std::endl;
            }
			std::cout << T_CB << "[" T_GNB << fd << T_CB "]" << " is requesting :" << T_N  << std::endl << requestBuffer << std::endl;
			running = clientRespond.finalRespond(fd, parsedRequest, env, Data);
			std::cout << "RUNNING: " << running << "\n";
        }
    }
    return (running);
}

std::vector<Config> configuration(int argc, char **argv) {
	std::string path;
	if (argc != 2)
		path = "config/default";
	else
		path = argv[1];
	Parser	file(path);
	file.setContentFile();
	file.checkGeneralSyntax();
	file.setConfiguration();
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
                running = processSockets(fd, Data, env); // reduire nbr arg, clean 
                break ;
            }
        }
    }
    closeAllFdUnlessMaster(Data.getReadSet(), Data.getTabMaster()); // Destructor destroys the master
    destroyTabMaster(Data.getTabMaster());
    return (0);
}