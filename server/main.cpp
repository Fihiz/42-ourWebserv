/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 11:59:24 by sad-aude          #+#    #+#             */
/*   Updated: 2021/06/29 13:08:24 by sad-aude         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "All.hpp"
#include "../config/Parser.hpp"
#include "../config/Config.hpp"

void    processMasterSocket(fd_set &readSet, std::vector<Socket *> tabMaster, int fd)
{
    int     clientSock;

    clientSock = accept(fd, NULL, NULL);
    if (clientSock < 0)
        error("Accept connexion", readSet, tabMaster);
    FD_SET(clientSock, &readSet);
    std::cout << T_BB "New connexion established on [" T_GNB << clientSock << T_BB "]" T_N << std::endl;
    return ;
}

int     processSockets(int fd, fd_set &readSet, std::vector<Socket *> tabMaster, char **env)
{
    char    requestBuffer[20000];
    int     running = 1;

    if (isTabMaster(tabMaster, fd) == 1)
        processMasterSocket(readSet, tabMaster, fd);
    else
    {
        ssize_t len = recv(fd, requestBuffer, 19999, 0); // Flags to check later
        if (len < 0)
            losingConnexion(fd, readSet, "Connexion lost... (");
        else
        {
            requestBuffer[len] = '\0';
			t_request	parsedRequest;
 
            (void) env;

			parsedRequest = parsingRequest(requestBuffer);
            checkingHeader(&parsedRequest);
			

            setContentDependingOnFileOrDirectory(parsedRequest);
            std::string responseToClient = "HTTP/1.1 200 OK\nContent-Type:" + parsedRequest.fileType + "\nContent-Length:" 
                                        + std::to_string(parsedRequest.fileContent.size()) + "\n\n" + parsedRequest.fileContent;
            if (parsedRequest.pathInfo == "./pages/exit.html") // (?)
                running = 0;
			std::cout << T_CB << "[" T_GNB << fd << T_CB "]" << " is requesting :" << T_N  << std::endl << requestBuffer << std::endl;
            // std::cout << T_YB << responseToClient << T_N << std::endl;
            if (send(fd, responseToClient.c_str(), responseToClient.size(), 0) < 0)
                error("Send", readSet, tabMaster);
            losingConnexion( fd, readSet, "Closing... [");
        }
    }
    return (running);
}

All configuration(const std::string & path) {
	Parser	file(path);
	file.setContent();
	file.checkSyntax();
	file.setConfiguration();
    file.setPorts();
    file.setHosts();
    file.setHostNames();
    All General(file.getConfiguration(), file.getListPorts(), file.getMapServerName(), file.getListHostNames());
	return (General);
}

int     main(int ac, char *av[], char *env[])
{
    (void)env;
    if (ac != 2) {
		std::cout << "Error: At least one argument is needed." << std::endl;
		return (1);
	}
	All General;
	try {
		General = configuration(av[1]);
	}
	catch (std::exception & err) {
		std::cout << err.what() << std::endl;
		return (1);
	}

	// // DISPLAY SERVER
	// int i = 1;
	// for (std::vector<Config>::const_iterator it = setup.begin(); it != setup.end(); it++)
	// {
	// 	std::cout << "-- [ " << "SETUP " << i++ << " ]" << std::endl;
	// 	it->printListen();
	// 	it->printServerName();
	// 	it->printMaxBodyClient();
	// 	it->printErrorPage();
	// 	it->printLocation();
	// 	std::cout << std::endl;
	// }
	// // ^

    fd_set readSet;
    fd_set readCopy;

    FD_ZERO(&readSet);
    FD_ZERO(&readCopy);
    
    std::vector<Socket *> tabMaster;

    std::list<int>  tempListPorts = General.getListPorts();
    std::list<int>::iterator it = tempListPorts.begin();
    std::list<int>::iterator ite = tempListPorts.end();
    while (it != ite) {
        Socket *master = new Socket(*it);
        tabMaster.push_back(master);
        FD_SET(master->getMasterSock(), &readSet);
        ++it;
    }

    int running = 1;

    while (running)
    {
        std::cout << T_GYB "\nWaiting in passive mode" T_N << std::endl;
        readCopy = readSet;
        if (select(FD_SETSIZE, &readCopy, 0, 0, 0) < 0)
            error("Select", readSet, tabMaster);
        for (int fd = 0; fd <= FD_SETSIZE; ++fd)
        {
            if (FD_ISSET(fd, &readCopy))
            {
                running = processSockets(fd, readSet, tabMaster, env);
                break ;
            }
        }
    }
    closeAllFdUnlessMaster(readSet, tabMaster); // Destructor destroys the master
    destroyTabMaster(tabMaster);
    return (0);
}