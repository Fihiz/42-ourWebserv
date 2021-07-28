/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseManager.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/28 11:48:55 by pgoudet           #+#    #+#             */
/*   Updated: 2021/07/28 17:05:25 by sad-aude         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "WebservData.hpp"
#include "../config/Parser.hpp"
#include "../config/Config.hpp"
#include <fstream>
#include <iostream>

void	printOutputs(int fd, t_request	parsedRequest, std::string clientRequest,std::string responseToClient)
{
	(void)parsedRequest;
	(void)clientRequest;
	(void)responseToClient;
	std::cout << T_CB << "[" T_GNB << fd << T_CB "]" << " is requesting :\n" << T_N << clientRequest << std::endl;
	// std::cout << "WE PRINT THE RESPONSE TO CLIENT HERE" << std::endl << T_YB << responseToClient.c_str() << T_N << "UNTIL HERE"<< std::endl;
	// std::cout << T_GYB "Current status code [" T_GNB << parsedRequest.statusCode << T_GYB << "]" << T_N << std::endl;
}

void	redirectCgiOutputToClient(t_request &req)
{
	int pid;

	char *argv[4];
	argv[0] = (char *)"./cgi-bin/php-cgi";
	argv[1] = (char *)"-q";
	argv[2] = const_cast<char *>(req.fullPathInfo.c_str());
	argv[3] = NULL;
	int fdTmp;

	fdTmp = open("transferCgi.html", O_RDWR | O_CREAT, 0777);
	pid = fork();
	if (pid == -1)
		std::cout << "ERROR" << std::endl;
	else if (pid == 0) {
		dup2(fdTmp, STDOUT_FILENO);
		if (execve(argv[0], argv, NULL) == -1) {
			std::cerr << "Internal Error" << " errno=" << errno << "\n";
			exit(1);
		}
		exit(0);
	}
	else {
		waitpid(pid, NULL, 0);
		close(fdTmp);
		
	}
	req.fileContent = getFileContent("transferCgi.html");
	remove("transferCgi.html");
}

std::string     buildClientResponse(t_request &parsedRequest, const t_location *locationBlock, Config *serverConfigBlock)
{
	std::string responseToClient;

	if (parsedRequest.statusCode == "301 Moved Permanently")
	{
		responseToClient = "HTTP/1.1 301 Moved Permanently\nLocation: " + parsedRequest.location;
	}
	else
	{
		parsedRequest.pathInfoCgi = "../cgi-bin/php-cgi"; // need to initialise in main ? 
		if (parsedRequest.fileExt == "php" && parsedRequest.pathInfoCgi.empty() == false && parsedRequest.statusCode == "200 OK")
		{
			redirectCgiOutputToClient(parsedRequest);
    		char    itoaTab[100];
			if (sprintf(itoaTab, "%lu", parsedRequest.fileContent.size()) > 0)
				responseToClient = "\nHTTP/1.1 " +  parsedRequest.statusCode + "\nContent-Type:" + parsedRequest.fileType + "\nContent-Length:" 
									+ (std::string)itoaTab + "\n\n" + parsedRequest.fileContent + "\r\n";
			else
				responseToClient = "\nHTTP/1.1 500 Internal Server Error\nContent-Type:text/html\nContent-Length:0\n\n\r\n";
		}
		else
		{
			if (parsedRequest.statusCode == "200 OK")
				setContentDependingOnFileOrDirectory(parsedRequest, locationBlock, serverConfigBlock);
			else
				parsedRequest.fileContent = getContentFileError(serverConfigBlock, parsedRequest.statusCode);
    		char    itoaTab[100];
			if (sprintf(itoaTab, "%lu", parsedRequest.fileContent.size()) > 0)
				responseToClient = "HTTP/1.1 " +  parsedRequest.statusCode + "\nContent-Type:" + parsedRequest.fileType + "\nContent-Length:" 
								+ (std::string)itoaTab + "\n\n" + parsedRequest.fileContent;
			else
				responseToClient = "\nHTTP/1.1 500 Internal Server Error\nContent-Type:text/html\nContent-Length:0\n\n\r\n";
		}
	}
	return (responseToClient);
}

void	sendResponseToClient(int fd, WebservData &Data, std::string &responseToClient)
{
	fcntl(fd, F_SETFL, O_NONBLOCK); // Keeping it ?
	if (send(fd, responseToClient.c_str(), responseToClient.size(), 0) <= 0)
		losingConnexion(fd, Data.getReadSet(), Data.getWriteSet(), "Client has been removed... [");
	losingConnexion( fd, Data.getReadSet(), Data.getWriteSet(), "Response has been sent, closing... [");
}