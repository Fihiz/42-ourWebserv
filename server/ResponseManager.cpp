/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseManager.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgoudet <pgoudet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/28 11:48:55 by pgoudet           #+#    #+#             */
/*   Updated: 2021/07/29 10:23:23 by pgoudet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "WebservData.hpp"
#include "../config/Parser.hpp"
#include "../config/Config.hpp"
#include <fstream>
#include <iostream>

int		checkpath(t_request &parsedRequest) {
	std::string path = parsedRequest.fullPathInfo;
	std::string parts;
	struct stat s;
	int st = -1;

	// /**/ std::cout << path << std::endl;

	int pos = -1;
	while (parsedRequest.statusCode == "200 OK" && (pos = path.find("/", pos + 1)) != (int)std::string::npos) {
		parts = path.substr(0, pos + 1);
		// /**/ std::cout << "Token: " << parts << std::endl;
		st = stat(parts.c_str(), &s);
		if ((S_ISDIR(s.st_mode) == 1) && (s.st_mode & S_IXOTH) != S_IXOTH) {
			parsedRequest.statusCode = "403 Forbidden";
		}
		else if ((S_ISREG(s.st_mode) == 1) && (s.st_mode & S_IROTH) != S_IROTH) { // (?)
			parsedRequest.statusCode = "401 Access Denied";
		}
	}
	if (parsedRequest.statusCode == "200 OK") {
		parts = path.substr(0, pos);
		// /**/ std::cout << "Token: " << parts << std::endl;
		st = stat(parts.c_str(), &s);
		if ((S_ISDIR(s.st_mode) == 1) && (s.st_mode & S_IXOTH) != S_IXOTH) {
			parsedRequest.statusCode = "403 Forbidden";
		}
		else if ((S_ISREG(s.st_mode) == 1) && (s.st_mode & S_IROTH) != S_IROTH) {
			parsedRequest.statusCode = "401 Access Denied";
		}
	}
	if (parsedRequest.statusCode == "200 OK")
		return (1);
	return (0);

	// /**/ std::cout << parsedRequest.statusCode << std::endl;
}

void	printOutputs(t_request	parsedRequest, std::string clientRequest,std::string responseToClient)
{
	(void)parsedRequest;
	(void)clientRequest;
	(void)responseToClient;
	//std::cout << T_CB << "[" T_GNB << fd << T_CB "]" << " is requesting :" << T_N << clientRequest << std::endl;
	// std::cout << "WE PRINT THE RESPONSE TO CLIENT HERE" << std::endl << T_YB << responseToClient.c_str() << T_N << "UNTIL HERE"<< std::endl;
	// std::cout << T_GYB "Current status code [" T_GNB << parsedRequest.statusCode << T_GYB << "]" << T_N << std::endl;
}

void	redirectCgiOutputToClient(t_request &req)
{
	int pid;

	char *argv[4];
	argv[0] = const_cast<char *>(req.pathInfoCgi.c_str());
	argv[1] = const_cast<char *>("-q");
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
	struct stat s;
	
	/* AFFICHAGE DEBUG */ std::cout << parsedRequest.requestMethod << std::endl;
	
	if (parsedRequest.requestMethod == "GET") {
		if (parsedRequest.statusCode == "301 Moved Permanently")
		{
			responseToClient = "HTTP/1.1 301 Moved Permanently\nLocation: " + parsedRequest.location;
		}
		else
		{
			parsedRequest.pathInfoCgi = serverConfigBlock->getCgi(serverConfigBlock->getRoutes(parsedRequest.route), parsedRequest.fileExt);
			if ((parsedRequest.fileExt == ".php" || parsedRequest.fileExt == ".py") && parsedRequest.statusCode == "200 OK")
			{
				
				if (stat(const_cast<char *>(parsedRequest.pathInfoCgi.c_str()), &s) == 0)
				{
					redirectCgiOutputToClient(parsedRequest);
				}
				else
				{
					parsedRequest.statusCode = "500 Internal Server Error";
					parsedRequest.fileContent = getContentFileError(serverConfigBlock, parsedRequest.statusCode);
					parsedRequest.fileType = "text / html";
				}
			}
			else
			{
				if (parsedRequest.statusCode == "200 OK")
					setContentDependingOnFileOrDirectory(parsedRequest, locationBlock, serverConfigBlock);
				else
					parsedRequest.fileContent = getContentFileError(serverConfigBlock, parsedRequest.statusCode);
			}
		}
	}
	else if (parsedRequest.requestMethod == "POST" && parsedRequest.statusCode == "200 OK") {
		std::cout << "METHOD POST ASKING !!" << std::endl;
	}
	else if (parsedRequest.requestMethod == "DELETE" && parsedRequest.statusCode == "200 OK") {
		if (checkpath(parsedRequest)) {
			if (remove(parsedRequest.fullPathInfo.c_str()) == -1)
				parsedRequest.statusCode = "500 Internal Server Error";
			else
				parsedRequest.statusCode = "204 No Content";
		}

		// int st = stat(const_cast<char *>(parsedRequest.fullPathInfo.c_str()), &s);
		// if (st != 0) {
		// 	parsedRequest.statusCode = "404 Not Found";
		// }
		// else {
		// 	if ((s.st_mode & S_IROTH) != S_IROTH)
		// 		parsedRequest.statusCode = "401 Access Denied";
		// 	else {
		// 		if (remove(parsedRequest.fullPathInfo.c_str()) == -1)
		// 			parsedRequest.statusCode = "500 Internal Server Error";
		// 		else
		// 			parsedRequest.statusCode = "204 No Content";
		// 	}
		// }
	}
	responseToClient = "\nHTTP/1.1 " +  parsedRequest.statusCode + "\nContent-Type:" + parsedRequest.fileType + "\nContent-Length:" 
		+ std::to_string(parsedRequest.fileContent.size()) + "\n\n" + parsedRequest.fileContent + "\r\n";
	return (responseToClient);

	// 204 No Content -> localhost/dossier/exist.html (fichier ou dossier)
	// 403 Forbidden -> localhost/dossier/ (si un element du chemin n'as pas les acces)
	// 404 Not Found -> localhost/dossier/noexist.html
	// 409 Conflict -> localhost/dossier

	// Dossier GET permission: execution
	// Fichier GET permission: lecture

	// Dossier DELETE permission: lecture sinon 500 Internal Server Error
	// Fichier DELETE permission: aucune
}

void	sendResponseToClient(int fd, WebservData &Data, std::string &responseToClient)
{
	fcntl(fd, F_SETFL, O_NONBLOCK); // Keeping it ?
	if (send(fd, responseToClient.c_str(), responseToClient.size(), 0) < 0)
		error("Send", Data);
	losingConnexion( fd, Data.getReadSet(), "Closing... [");
}