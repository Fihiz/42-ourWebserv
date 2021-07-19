/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 11:59:24 by sad-aude          #+#    #+#             */
/*   Updated: 2021/07/19 15:26:19 by sad-aude         ###   ########lyon.fr   */
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
    {
        std::cout << "Config has been found" << std::endl;
        return (&(Data.getMapServerName()[host]));
    }
    return (NULL);
}



int     processSockets(int fd, WebservData &Data, char **env)
{
    (void) Data;
    char    requestBuffer[20000];
    int     running = 1;

    if (isTabMaster(Data.getTabMaster(), fd) == 1)
        processMasterSocket(Data, fd);
    else
    {
        ssize_t len = recv(fd, requestBuffer, 19999, 0); // Flags to check later
        if (len < 0)
            losingConnexion(fd, Data.getReadSet(), "Connexion lost... (");
        else
        {
            requestBuffer[len] = '\0';
			t_request	parsedRequest;
 
            (void) env;

			parsedRequest = parsingRequest(requestBuffer);
            Config *configForClient;
            
            //std::cout << "Into configForClient, host: " << parsedRequest.host << std::endl;
            configForClient = findConfigForClient(Data, parsedRequest.host);
            if (!configForClient)
            {
                parsedRequest.statusCode = "400 Bad Request";
                parsedRequest.pathInfo = "./pages/400.html";
            }
            else
            {
                parsedRequest.fullPathInfo = configForClient->getRoot("") + parsedRequest.pathInfo.substr(2);
                std::cout << "PATH : " << parsedRequest.fullPathInfo << std::endl; 
                
                // const t_location  *locationForClient;
                // to do :comparer les location et choisir la plus coherente
                // locationForClient = configForClient->getLocation("/"); // temporaire
                // if (locationForClient)
                //     checkingHeader(&parsedRequest, locationForClient->method);
                std::vector<std::string>    method;
                method.push_back("GET");
                checkingHeader(&parsedRequest, method);
                std::cout << "PORT CONFIG : ";
                configForClient->printListen();
                std::cout << "HOST NAME : " << configForClient->getServerName() << std::endl;                
            }
            // std::cout << T_GYB "Current status code [" << parsedRequest.statusCode << "]" << T_N << std::endl;
            
            setContentDependingOnFileOrDirectory(parsedRequest);
             
            std::string responseToClient = "HTTP/1.1 " +  parsedRequest.statusCode + "\nContent-Type:" + parsedRequest.fileType + "\nContent-Length:" 
                                        + std::to_string(parsedRequest.fileContent.size()) + "\n\n" + parsedRequest.fileContent;
            if (parsedRequest.pathInfo == "./exit.html") // (?)
                running = 0;
			std::cout << T_CB << "[" T_GNB << fd << T_CB "]" << " is requesting :" << T_N  << std::endl << requestBuffer << std::endl;
            // std::cout << "WE PRINT THE RESPONSE TO CLIENT HERE" << std::endl << T_YB << responseToClient.c_str() << T_N << "UNTIL HERE"<< std::endl;
            std::cout << T_GYB "Current status code [" << parsedRequest.statusCode << "]" << T_N << std::endl;
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