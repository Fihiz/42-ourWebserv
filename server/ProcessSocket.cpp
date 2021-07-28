/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcessSocket.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/28 11:11:42 by pgoudet           #+#    #+#             */
/*   Updated: 2021/07/28 15:12:44 by sad-aude         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "WebservData.hpp"
#include "Webserv.hpp"
#include "../config/Parser.hpp"
#include "../config/Config.hpp"
#include <fstream>
#include <iostream>

int     processClientSocket(WebservData &Data, int fd)
{
	int running = 1;
	std::string clientRequest;

	if (receiveClientRequest(fd, clientRequest) < 0)
		losingConnexion(fd, Data.getReadSet(), Data.getWriteSet(), "Connexion lost... (");
	else
	{
		t_request	parsedRequest = parsingRequest(clientRequest);

		if (parsedRequest.statusCode != "200 OK")
			return (running);

		Config *serverConfigBlock = findServerConfigBlock(Data, parsedRequest.host);
		const t_location  *locationBlock = NULL;

		checkServerConfigBlock(parsedRequest, serverConfigBlock);
		checkRedir(serverConfigBlock, parsedRequest);
		locationBlock = checkLocationBlock(parsedRequest, serverConfigBlock);
		
		std::string responseToClient = buildClientResponse(parsedRequest, locationBlock, serverConfigBlock);

		if (parsedRequest.pathInfo == "/exit.html")
			running = 0;

		printOutputs(parsedRequest, clientRequest, responseToClient);
		//std::cout << " \r \r \r";
		sendResponseToClient(fd, Data, responseToClient);
	}
	return (running);
}

void    processMasterSocket(WebservData &Data, int fd)
{
	int     clientSock;

	clientSock = accept(fd, NULL, NULL);
	if (clientSock < 0)
		error("Accept connexion", Data);
	FD_SET(clientSock, &Data.getReadSet());
	FD_SET(clientSock, &Data.getWriteSet());
	std::cout << T_BB "New connexion established on [" T_GNB << clientSock << T_BB "]\n" T_N << std::endl;
	return ;
}

int     processSockets(int fd, WebservData &Data)
{
	int     running = 1;

	if (isTabMaster(Data.getTabMaster(), fd) == 1)
		processMasterSocket(Data, fd);
	else
		running = processClientSocket(Data, fd);
	return (running);
}