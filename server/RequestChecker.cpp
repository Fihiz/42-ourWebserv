/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestChecker.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgoudet <pgoudet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/28 11:31:37 by pgoudet           #+#    #+#             */
/*   Updated: 2021/07/28 11:34:38 by pgoudet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "WebservData.hpp"
#include "../config/Parser.hpp"
#include "../config/Config.hpp"
#include <fstream>
#include <iostream>

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