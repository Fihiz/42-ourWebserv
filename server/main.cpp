/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgoudet <pgoudet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 11:59:24 by sad-aude          #+#    #+#             */
/*   Updated: 2021/07/28 11:14:44 by pgoudet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "WebservData.hpp"
#include "../config/Parser.hpp"
#include "../config/Config.hpp"
#include <fstream>
#include <iostream>

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


Config *findServerConfigBlock(WebservData &Data, std::string host)
{
	if (host.find(":") == std::string::npos)
	{
	   host = host + ":80"; 
	}
	if (Data.getMapServerName().find(host) != Data.getMapServerName().end())
		return (&(Data.getMapServerName()[host]));
	return (NULL);
}

void    setFullPathInfo(t_request &parsedRequest, Config &serverConfigBlock, std::string &tmpFile)
{
	parsedRequest.fullPathInfo = parsedRequest.pathInfo;
	parsedRequest.fullPathInfo.replace(0, tmpFile.size(), serverConfigBlock.getRoot(tmpFile));
}

const t_location *findLocationBlock(Config &serverConfigBlock, t_request &parsedRequest)
{
	const t_location  *locationBlock = NULL;
	std::string tmpFile = parsedRequest.pathInfo;

	while(tmpFile.size() > 0 && locationBlock == NULL)
	{
		locationBlock = serverConfigBlock.getLocation(tmpFile);
		if (!locationBlock)
			tmpFile.resize(tmpFile.size() - 1);
	}
	setFullPathInfo(parsedRequest, serverConfigBlock, tmpFile);
	checkingMethod(parsedRequest, locationBlock->method);

	return (locationBlock);
}

void    checkRedir(Config *serverConfigBlock, t_request &parsedRequest)
{
	if (parsedRequest.pathInfo[parsedRequest.pathInfo.size() - 1] != '/')
	{
		struct stat statBuf;
		int ret = stat((parsedRequest.pathInfo).c_str(), &statBuf);
		if ((ret != -1 && S_ISDIR(statBuf.st_mode)) || (serverConfigBlock->getLocation(parsedRequest.pathInfo + "/")))
		{
			parsedRequest.statusCode = "301 Moved Permanently";
			parsedRequest.location = parsedRequest.pathInfo + "/";
		}
	}
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

off_t     getFdSize(int fd)
{
	struct stat buf;
	fstat(fd, &buf);
	off_t fdSize = buf.st_size;
	std::cout << T_BB "SIZE OF FD = " << fdSize << T_N << std::endl;
	return (fdSize);
}

ssize_t     receiveClientRequest(int fd, std::string &clientRequest)
{
	char    requestBuffer[1];
	ssize_t len = 1;
	off_t fdSize = getFdSize(fd);
	while ((clientRequest.size() < (size_t)fdSize) && ((len = recv(fd, requestBuffer, 1, 0)) >= 0) )
	{
		std::cout << " \r \r \r";
		clientRequest =  clientRequest  + requestBuffer[0];
		if (len == 0)
			break;
	}
	clientRequest = clientRequest + '\0';
	return (len);
}

void    checkServerConfigBlock(t_request &parsedRequest, Config *serverConfigBlock)
{
	if (!serverConfigBlock)
		parsedRequest.statusCode = "400 Bad Request";
	else
	{
		if ((unsigned long)atoi(parsedRequest.contentLenght.c_str()) > serverConfigBlock->getMaxBodySize())
		{
			parsedRequest.statusCode = "413 Request Entity Too Large"; // A changer ?
			return ;
		}
	   
	}
}

const t_location  *checkLocationBlock(t_request &parsedRequest, Config *serverConfigBlock)
{
	const t_location  *locationBlock = NULL;
	
	if (parsedRequest.statusCode == "200 OK")
	{
		locationBlock = findLocationBlock(*serverConfigBlock, parsedRequest);
		if ((parsedRequest.statusCode != "400 Bad Request")
			&& (locationBlock->redirect != ""))
		{
			parsedRequest.statusCode = "301 Moved Permanently";
			parsedRequest.location = locationBlock->redirect;
		}
		//std::cout << "PATH : " << parsedRequest.fullPathInfo << std::endl;        
	}
	return (locationBlock);
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
			responseToClient = "\nHTTP/1.1 " +  parsedRequest.statusCode + "\nContent-Type:" + parsedRequest.fileType + "\nContent-Length:" 
									+ std::to_string(parsedRequest.fileContent.size()) + "\n\n" + parsedRequest.fileContent + "\r\n";
		}
		else
		{
			if (parsedRequest.statusCode == "200 OK")
				setContentDependingOnFileOrDirectory(parsedRequest, locationBlock, serverConfigBlock);
			else
				parsedRequest.fileContent = getContentFileError(serverConfigBlock, parsedRequest.statusCode);
			responseToClient = "HTTP/1.1 " +  parsedRequest.statusCode + "\nContent-Type:" + parsedRequest.fileType + "\nContent-Length:" 
								+ std::to_string(parsedRequest.fileContent.size()) + "\n\n" + parsedRequest.fileContent;          
		}
	}
	return (responseToClient);
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

void	sendResponseToClient(int fd, WebservData &Data, std::string &responseToClient)
{
	fcntl(fd, F_SETFL, O_NONBLOCK); // Keeping it ?
	if (send(fd, responseToClient.c_str(), responseToClient.size(), 0) < 0)
		error("Send", Data);
	losingConnexion( fd, Data.getReadSet(), "Closing... [");
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

	//file.printConfigs();

	return (file.getConfiguration());
}

void    serverLoop(WebservData &Data)
{
	int running = 1;

	while (running)
	{
		std::cout << T_GYB "Waiting in passive mode" T_N << std::endl;
		Data.getReadCopy() = Data.getReadSet();
		if (select(FD_SETSIZE, &Data.getReadCopy(), 0, 0, 0) < 0)
			error("Select", Data);
		else
			for (int fd = 0; fd <= FD_SETSIZE; ++fd)
			{
				if (FD_ISSET(fd, &Data.getReadCopy()))
				{
					running = processSockets(fd, Data);
					break ;
				}
			}
	}
}

int     main(int ac, char *av[])
{
	std::vector<Config> setup;
	try {
		setup = configuration(ac, av);
	}
	catch (std::exception & err) {
		std::cout << err.what() << std::endl;
		return (1);
	}

	WebservData Data(setup);
	std::cout << std::endl;

	serverLoop(Data);
	closeAllFdUnlessMaster(Data.getReadSet(), Data.getTabMaster()); // Destructor destroys the master
	destroyTabMaster(Data.getTabMaster());
	return (0);
}