/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestManager.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/28 11:24:28 by pgoudet           #+#    #+#             */
/*   Updated: 2021/07/30 17:35:31 by sad-aude         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "WebservData.hpp"
#include "../config/Parser.hpp"
#include "../config/Config.hpp"
#include <fstream>
#include <iostream>

void			setFullPathInfo(t_request &parsedRequest, Config &serverConfigBlock, std::string &tmpFile)
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
	parsedRequest.route = tmpFile;
	setFullPathInfo(parsedRequest, serverConfigBlock, tmpFile);
	checkingMethod(parsedRequest, locationBlock->method);

	return (locationBlock);
}

Config			*findServerConfigBlock(WebservData &Data, std::string host)
{
	if (host.find(":") == std::string::npos)
	   host = host + ":80";
	if (Data.getMapServerName().find(host) != Data.getMapServerName().end())
		return (&(Data.getMapServerName()[host]));
	return (NULL);
}


off_t			getFdSize(int fd)
{
	struct stat buf;
	fstat(fd, &buf);
	off_t fdSize = buf.st_size;
	std::cout << T_BB "SIZE OF FD = " << fdSize << T_N << std::endl;
	return (fdSize);
}

ssize_t			receiveClientRequest(int fd, std::string &clientRequest)
{
	char    requestBuffer[1];
	ssize_t len = -1;
	off_t fdSize = getFdSize(fd);
	while ((clientRequest.size() < (size_t)fdSize) && ((len = recv(fd, requestBuffer, 1, 0)) >= 0) )
	{
		clientRequest =  clientRequest  + requestBuffer[0];
		if (len == 0)
			break;
	}
	clientRequest = clientRequest + '\0';
	return (len);
}