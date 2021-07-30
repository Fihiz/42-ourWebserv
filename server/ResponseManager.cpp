/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseManager.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/28 11:48:55 by pgoudet           #+#    #+#             */
/*   Updated: 2021/07/30 16:33:58 by sad-aude         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "WebservData.hpp"
#include "../config/Parser.hpp"
#include "../config/Config.hpp"
#include <fstream>
#include <iostream>

int        checkPath(t_request &parsedRequest) {
    std::string path = parsedRequest.fullPathInfo;
    std::string parts;
    struct stat s;

    int st = -1;
    int pos = -1;
    while (parsedRequest.statusCode == "200 OK" && (pos = path.find("/", pos + 1)) != (int)std::string::npos) {
        parts = path.substr(0, pos + 1);
        st = stat(parts.c_str(), &s);
        if (st != 0) {
            parsedRequest.statusCode = "404 Not Found";
        }
        else if ((S_ISDIR(s.st_mode) == 1) && (s.st_mode & S_IXOTH) != S_IXOTH) {
            parsedRequest.statusCode = "403 Forbidden";
        }
        else if ((S_ISREG(s.st_mode) == 1) && (s.st_mode & S_IROTH) != S_IROTH) {
            parsedRequest.statusCode = "403 Forbidden";
        }
    }
    if (parsedRequest.statusCode == "200 OK") {
        parts = path.substr(0, pos);
        st = stat(parts.c_str(), &s);
        if (st != 0) {
            parsedRequest.statusCode = "404 Not Found";
        }
        else if ((S_ISDIR(s.st_mode) == 1) && (s.st_mode & S_IXOTH) != S_IXOTH) {
            parsedRequest.statusCode = "403 Forbidden";
        }
        else if ((S_ISREG(s.st_mode) == 1) && (s.st_mode & S_IROTH) != S_IROTH) {
            parsedRequest.statusCode = "403 Forbidden";
        }
    }
    if (parsedRequest.statusCode == "200 OK")
        return (1);
    return (0);
}

void	printOutputs(int fd, t_request	parsedRequest, std::string clientRequest,std::string responseToClient)
{
	(void)parsedRequest;
	(void)clientRequest;
	(void)responseToClient;
	(void)fd;
	// std::cout << T_CB << "Client [" T_GNB << fd << T_CB "]" << " is requesting :\n" << T_N << clientRequest << std::endl;
	// std::cout << "WE PRINT THE RESPONSE TO CLIENT HERE" << std::endl << T_YB << responseToClient.c_str() << T_N << "UNTIL HERE"<< std::endl;
	std::cout << T_GYB "Current status code [" T_GNB << parsedRequest.statusCode << T_GYB << "]" << T_N << std::endl;
}

/* OLD */
// void			redirectCgiOutputToClient(t_request &req)
// {
// 	int pid;
// 	char *argv[4];
// 	int fdTmp;

// 	if  (req.fileExt == ".py" || req.fileExt == ".pl")
// 	{
// 		argv[0] = const_cast<char *>(req.pathInfoCgi.c_str());
// 		argv[1] = const_cast<char *>(req.fullPathInfo.c_str());
// 		argv[2] = NULL;
// 	}
// 	else
// 	{
// 		argv[0] = const_cast<char *>(req.pathInfoCgi.c_str());
// 		argv[1] = const_cast<char *>("-q");
// 		argv[2] = const_cast<char *>(req.fullPathInfo.c_str());
// 		argv[3] = NULL;
// 	}

	
// 	fdTmp = open("transferCgi.html", O_RDWR | O_CREAT, 0777);
// 	pid = fork();
// 	if (pid == -1)
// 		std::cout << "ERROR" << std::endl;
// 	else if (pid == 0) {
// 		dup2(fdTmp, STDOUT_FILENO);
// 		if (execve(argv[0], argv, NULL) == -1) {
// 			std::cerr << "Internal Error" << " errno=" << errno << "\n";
// 			exit(1);
// 		}
// 		exit(0);
// 	}
// 	else {
// 		waitpid(pid, NULL, 0);
// 		close(fdTmp);
		
// 	}
// 	req.fileContent = getFileContent("transferCgi.html");
// 	remove("transferCgi.html");
// }

void            redirectCgiOutputToClient(t_request &req, Config *serverConfigBlock)
{
    int pid = -1;
    char *argv[4];
    int fdTmp = -1;
    int status = 0;

    if  (req.fileExt == ".py" || req.fileExt == ".pl")
    {
        argv[0] = const_cast<char *>(req.pathInfoCgi.c_str());
        argv[1] = const_cast<char *>(req.fullPathInfo.c_str());
        argv[2] = NULL;
    }
    else
    {
        argv[0] = const_cast<char *>(req.pathInfoCgi.c_str());
        argv[1] = const_cast<char *>("-q");
        argv[2] = const_cast<char *>(req.fullPathInfo.c_str());
        argv[3] = NULL;
    }

    
    fdTmp = open("transferCgi.html", O_RDWR | O_CREAT, 0777);
    // fdTmp = -1; // Simule crash
    if (fdTmp != -1) {
        pid = fork();
        if (pid == -1)
            std::cout << "ERROR" << std::endl;
        else if (pid == 0) {
            dup2(fdTmp, STDOUT_FILENO);
            exit(execve(argv[0], argv, NULL));
        }
        else {
            wait4(pid, &status, status, NULL);
            close(fdTmp);    
        }
        req.fileContent = getFileContent("transferCgi.html");
        if (status != 0)
        {
            req.statusCode = "500 Internal Server Error";
            req.fileContent = getContentFileError(serverConfigBlock, req.statusCode);
            req.fileType = "text / html";
        }
        remove("transferCgi.html");
    }
    else
    {
        req.statusCode = "500 Internal Server Error";
        req.fileContent = getContentFileError(serverConfigBlock, req.statusCode);
        req.fileType = "text / html";
    }
}

std::string     buildClientResponse(t_request &parsedRequest, const t_location *locationBlock, Config *serverConfigBlock)
{
	std::string responseToClient;

	if (parsedRequest.requestMethod != "DELETE" || parsedRequest.statusCode != "200 OK") {
		if (parsedRequest.statusCode == "301 Moved Permanently")
			return ("HTTP/1.1 301 Moved Permanently\nLocation: " + parsedRequest.location);
		else
		{
			parsedRequest.pathInfoCgi = serverConfigBlock->getCgi(serverConfigBlock->getRoutes(parsedRequest.route), parsedRequest.fileExt);
			if ((parsedRequest.fileExt == ".php" || parsedRequest.fileExt == ".pl" || parsedRequest.fileExt == ".py") \
			 	&& checkPath(parsedRequest) && parsedRequest.pathInfoCgi.empty() == false)
				if (checkPath(parsedRequest))
					redirectCgiOutputToClient(parsedRequest, serverConfigBlock);
				else
				{
					std::cout << checkPath(parsedRequest) << std::endl;
					std::cout << parsedRequest.statusCode << "\n";
					parsedRequest.statusCode = "500 Internal Server Error";
					parsedRequest.fileContent = getContentFileError(serverConfigBlock, parsedRequest.statusCode);
					parsedRequest.fileType = "text / html";
				}
			else
                if (checkPath(parsedRequest))
                    setContentDependingOnFileOrDirectory(parsedRequest, locationBlock, serverConfigBlock);
                else
                    parsedRequest.fileContent = getContentFileError(serverConfigBlock, parsedRequest.statusCode);
		}
	}
    else
    {
        if (checkPath(parsedRequest))
        {
            if (remove(parsedRequest.fullPathInfo.c_str()) == -1)
				parsedRequest.statusCode = "500 Internal Server Error";
			else
				parsedRequest.statusCode = "204 No Content";
        }
        else
            parsedRequest.fileContent = getContentFileError(serverConfigBlock, parsedRequest.statusCode);
    }
	char    itoaTab[100];
	if (sprintf(itoaTab, "%lu", parsedRequest.fileContent.size()) > 0)
		responseToClient = "HTTP/1.1 " +  parsedRequest.statusCode + "\nContent-Type:" + parsedRequest.fileType + "\nContent-Length:" 
							+ (std::string)itoaTab + "\n\n" + parsedRequest.fileContent + "\r\n";
	else
		responseToClient = "\nHTTP/1.1 500 Internal Server Error\nContent-Type:text/html\nContent-Length:0\n\n\r\n";
	return (responseToClient);
}

void			sendResponseToClient(int fd, WebservData &Data, std::string &responseToClient)
{
	fcntl(fd, F_SETFL, O_NONBLOCK);
    std::cout << responseToClient << std::endl;
	if (send(fd, responseToClient.c_str(), responseToClient.size(), 0) <= 0)
		losingConnexion(fd, Data.getReadSet(), Data.getWriteSet(), "Client has been removed... [");
	losingConnexion( fd, Data.getReadSet(), Data.getWriteSet(), "Response has been sent, closing... [");
}