/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgoudet <pgoudet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 11:59:24 by sad-aude          #+#    #+#             */
/*   Updated: 2021/07/24 10:48:53 by pgoudet          ###   ########.fr       */
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
	int p[2];
	int pid;
	
	pipe(p);
	
	char *argv2[4];
	argv2[0] = (char *)"./cgi-bin/php-cgi";
	argv2[1] = (char *)"-q";
	argv2[2] = const_cast<char *>(req.fullPathInfo.c_str());
	argv2[3] = NULL;

	std::string str, str1;
	str1.resize(1024);
	
	(void)p;
	(void)str;
	(void)str1;
	(void)env;
	(void)fd;
	// (void)req;
	(void)resp;
	(void)Data;
	
	pid = fork();

	// if (pid == 0)
	// {
	// 	// if (stat("./cgi-bin/php-cgi") != 0)
	// 	// 	std::cerr << "test" << "errno=" << errno << "\n";
	// 	close(p[0]);
	// 	dup2(p[1], fd);
	// 	(void)req;
	// 	(void)resp;
	// 	(void)Data;
	// 	if (execve(argv2[0], argv2, env) == -1)
	// 	{
	// 		// resp.setStatusCode("500 Internal Error");
	// 		// resp.finalRespond(fd, req, env, Data);
	// 		std::cerr << "Internal Error" << " errno=" << errno << "\n";
	// 		close(p[1]);
	// 		exit(0);
	// 	}
	// 	close(p[1]);
	// 	exit (0);
	// }
	// else
	// {
	// 	waitpid(-1, NULL, 0);
	// 	close(p[1]);
	// 	dup2(p[0], 1);
	// 	while(read(p[0], &str1[0], 1024) == 1024)
	// 	{
	// 		str  = str + str1;
	// 		str1.clear();
	// 		str1.resize(1024);
	// 	}
	// 	str  = str + str1;
	// 	send(p[0], str.c_str(), str.length(), 0);
	// }

	/************************/

	// // VERSION AVEC PIPE
	if (pid == -1)
		std::cout << "ERROR" << std::endl;
	else if (pid == 0) {
		close(p[0]); // READ SIDE
		dup2(fd, STDOUT_FILENO);
		if (execve(argv2[0], argv2, env) == -1) {
			std::cerr << "Internal Error" << " errno=" << errno << "\n";
			close(p[1]);
			exit(1);
		}
		close(p[1]);
		exit(0);
	}
	else {
		close(p[1]); // WRITE SIDE
		waitpid(pid, NULL, 0);
		
		struct stat buf;
		fstat(fd, &buf);
    	off_t fdSize = buf.st_size;
		char tmp[fdSize];
		read(fd, tmp, fdSize);
		req.fileContent = tmp;
		req.fileContent += "\0";
		close(p[0]);
	}

	// // VERSION SANS PIPE
	// if (pid == -1)
	// 	std::cout << "ERROR" << std::endl;
	// else if (pid == 0) {
	// 	dup2(fd, STDOUT_FILENO);
	// 	if (execve(argv2[0], argv2, env) == -1) {
	// 		std::cerr << "Internal Error" << " errno=" << errno << "\n";
	// 		exit(1);
	// 	}
	// 	exit(0);
	// }
	// else {
	// 	waitpid(pid, NULL, 0);
	// }

	// /**/ SIMPLE TEST /**/
	// if (pid == -1)
	// 	std::cout << "ERROR" << std::endl;
	// else if (pid == 0) {
	// 	close(p[0]); // READ SIDE
	// 	dup2(p[1], STDOUT_FILENO);
	// 	write(p[1], "salut\n", strlen("salut\n"));
	// 	close(p[1]);
	// 	exit(0);
	// }
	// else {
	// 	close(p[1]); // WRITE SIDE
	// 	waitpid(pid, NULL, 0);
	// 	char buffer[6];
	// 	read(p[0], buffer, 6);
	// 	buffer[5] = '\0';
	// 	close(p[0]);
	// 	dprintf(1, "%s\n", buffer);
	// }
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