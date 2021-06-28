/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobenass <jobenass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 11:59:24 by sad-aude          #+#    #+#             */
/*   Updated: 2021/06/28 18:13:51 by jobenass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "../config/Parser.hpp"
#include "../config/Config.hpp"
#include "./All.hpp"

void    processMasterSocket(fd_set &read_set, std::vector<Socket *> tabMaster, int fd)
{
    int     clientSock;

    clientSock = accept(fd, NULL, NULL);
    if (clientSock < 0)
        error("Accept connexion", read_set, tabMaster);
    FD_SET(clientSock, &read_set);
    std::cout << T_BB "New connexion established on [" << clientSock << "]" T_N << std::endl;
    return ;
}

int     processSockets(int fd, fd_set &read_set, std::vector<Socket *> tabMaster, char **env)
{
    char    requestBuffer[20000];
    int     running = 1;

    if (isTabMaster(tabMaster, fd) == 1)
        processMasterSocket(read_set, tabMaster, fd);
    else
    {
        ssize_t len = recv(fd, requestBuffer, 19999, 0); // Flags to check later
        if (len < 0)
            losingConnexion(fd, read_set, "Connexion lost... (");
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
			std::cout << T_CB << "[" << fd << "]" << " is requesting :" << T_N  << std::endl << requestBuffer << std::endl;
            // std::cout << T_YB << responseToClient << T_N << std::endl;
            if (send(fd, responseToClient.c_str(), responseToClient.size(), 0) < 0)
                error("Send", read_set, tabMaster);
            losingConnexion( fd, read_set, "Closing... [");
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
    All General(file.getConfiguration(), file.getListPorts(), file.getMapServerName());
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
		General = configuration(av[1], );
        std::cout << "Port 1: " << General.getListPorts().front() << std::endl;
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

    fd_set read_set;
    fd_set read_copy;

    FD_ZERO(&read_set);
    FD_ZERO(&read_copy);
    
    std::vector<Socket *> tabMaster;

    std::list<int>::iterator it = General.begin();
    while (it != setup.end()) {
        Socket *master = new Socket( it->getListen() );
        std::cout << it->getListen() << std::endl;
        tabMaster.push_back(master);
        FD_SET(master->getMasterSock(), &read_set);
        ++it;
    }

    int running = 1;

    while (running)
    {
        std::cout << T_GYB "Waiting in passive mode" T_N << std::endl;
        read_copy = read_set;
        if (select(FD_SETSIZE, &read_copy, 0, 0, 0) < 0)
            error("Select", read_set, tabMaster);
        for (int fd = 0; fd <= FD_SETSIZE; ++fd)
        {
            if (FD_ISSET(fd, &read_copy))
            {
                running = processSockets(fd, read_set, tabMaster, env);
                break ;
            }
        }
    }
    closeAllFdUnlessMaster(read_set, tabMaster); // Destructor destroys the master
    destroyTabMaster(tabMaster);
    return (0);
}