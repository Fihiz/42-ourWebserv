/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agathe <agathe@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 11:59:24 by sad-aude          #+#    #+#             */
/*   Updated: 2021/07/06 16:18:17 by agathe           ###   ########lyon.fr   */
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

void    processMasterSocket(fd_set &readSet, std::vector<Socket *> tabMaster, int fd, std::map<int, int> &portLinkedToClientFd)
{
    int     clientSock;

    clientSock = accept(fd, NULL, NULL);
    if (clientSock < 0)
        error("Accept connexion", readSet, tabMaster);
    FD_SET(clientSock, &readSet);


    
    std::cout << "client sock : " << clientSock << std::endl;
    std::cout << "fd : " << fd << std::endl;
    
    std::cout << "port master : " << findSpecificMasterSocket(tabMaster, fd)->getUsedPort() << std::endl;
    
    portLinkedToClientFd[clientSock] = findSpecificMasterSocket(tabMaster, fd)->getUsedPort();
    
    std::cout << "portLinkedToClientFd : " << portLinkedToClientFd[clientSock] << std::endl;
    
    
    std::cout << T_BB "New connexion established on [" T_GNB << clientSock << T_BB "]\n" T_N << std::endl;
    return ;
}


Config *findConfigForClient(WebservData &Data, std::string host)
{
    std::string test = (std::string)host.c_str();
    std::cout << "start host : " << host << std::endl;

    std::string eighty = ":80";
    //host = (std::string)"LOCALHOST" + (std::string)".500" ;
    
    if (test.find(":") == std::string::npos)
    {
       test = test + eighty; 
    }

    std::cout << "0 test 00 : |" << (int)test[9] << "|" << std::endl;
    
 
    
    // std::cout << "port : " << host << std::endl;

    if (Data.getMapServerName().find(host) != Data.getMapServerName().end())
    {
        std::cout << "1 test 11 : " << test << std::endl;
        return (&(Data.getMapServerName()[host]));
    }
    std::cout << "2 test 22 : " << test << std::endl;
    return (NULL);
}



int     processSockets(int fd, fd_set &readSet, std::vector<Socket *> tabMaster, char **env, std::map<int, int> &portLinkedToClientFd, WebservData &Data)
{
    (void) Data;
    char    requestBuffer[20000];
    int     running = 1;

    if (isTabMaster(tabMaster, fd) == 1)
        processMasterSocket(readSet, tabMaster, fd, portLinkedToClientFd);
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
            
            Config *configForClient;
            
            configForClient = findConfigForClient(Data, parsedRequest.host);

            
            std::string responseToClient = "HTTP/1.1 " +  parsedRequest.statusCode + "\nContent-Type:" + parsedRequest.fileType + "\nContent-Length:" 
                                        + std::to_string(parsedRequest.fileContent.size()) + "\n\n" + parsedRequest.fileContent;
            if (parsedRequest.pathInfo == "./pages/exit.html") // (?)
                running = 0;
			std::cout << T_CB << "[" T_GNB << fd << T_CB "]" << " is requesting :" << T_N  << std::endl << requestBuffer << std::endl;
            std::cout << "WE PRINT THE RESPONSE TO CLIENT HERE" << std::endl << T_YB << responseToClient.c_str() << T_N << "UNTIL HERE"<< std::endl;
            if (send(fd, responseToClient.c_str(), responseToClient.size(), 0) < 0)
                error("Send", readSet, tabMaster);
            losingConnexion( fd, readSet, "Closing... [");
            portLinkedToClientFd.erase(portLinkedToClientFd.find(fd));
            // std::cout << "portLinkedToClientFd AFTER ERASE: " << portLinkedToClientFd[fd] << std::endl;
            
        }
    }
    return (running);
}

std::vector<Config> configuration(const std::string & path) {
	Parser	file(path);
	file.setContent();
	file.checkSyntax();
	file.setConfiguration();
	return (file.getConfiguration());
}

int     main(int ac, char *av[], char *env[])
{
    (void)env;
    if (ac != 2) {
		std::cout << "Error: At least one argument is needed." << std::endl;
		return (1);
	}
	std::vector<Config> setup;
	try {
		setup = configuration(av[1]);
	}
	catch (std::exception & err) {
		std::cout << err.what() << std::endl;
		return (1);
	}

    fd_set readSet;
    fd_set readCopy;

    FD_ZERO(&readSet);
    FD_ZERO(&readCopy);

    WebservData Data(setup);
    
    std::vector<Socket *> tabMaster;

    std::map<int, int> portLinkedToClientFd;

    Data.setPorts();
    std::list<int>  tempListPorts = Data.getListPorts();
    std::list<int>::iterator it = tempListPorts.begin();
    std::list<int>::iterator ite = tempListPorts.end();

    while (it != ite) {
        Socket *master = new Socket(*it);
        tabMaster.push_back(master);
        FD_SET(master->getMasterSock(), &readSet);
        ++it;
    }

    int running = 1;
    std::cout << std::endl;

    while (running)
    {
        std::cout << T_GYB "Waiting in passive mode" T_N << std::endl;
        readCopy = readSet;
        if (select(FD_SETSIZE, &readCopy, 0, 0, 0) < 0)
            error("Select", readSet, tabMaster);
        for (int fd = 0; fd <= FD_SETSIZE; ++fd)
        {
            if (FD_ISSET(fd, &readCopy))
            {
                running = processSockets(fd, readSet, tabMaster, env, portLinkedToClientFd, Data); // reduire nbr arg, clean 
                break ;
            }
        }
    }
    closeAllFdUnlessMaster(readSet, tabMaster); // Destructor destroys the master
    destroyTabMaster(tabMaster);
    return (0);
}