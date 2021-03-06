/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestChecker.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/28 11:31:37 by pgoudet           #+#    #+#             */
/*   Updated: 2021/07/30 18:17:01 by sad-aude         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "WebservData.hpp"
#include "../config/Parser.hpp"
#include "../config/Config.hpp"
#include <fstream>
#include <iostream>

void				checkRedir(Config *serverConfigBlock, t_request &parsedRequest)
{
	if (parsedRequest.statusCode == "200 OK" && parsedRequest.pathInfo[parsedRequest.pathInfo.size() - 1] != '/')
	{
		struct stat statBuf;

		int ret = stat((parsedRequest.fullPathInfo).c_str(), &statBuf);
		if ((ret != -1 && S_ISDIR(statBuf.st_mode)) || (serverConfigBlock->getLocation(parsedRequest.pathInfo + "/")))
		{
			parsedRequest.statusCode = "301 Moved Permanently";
			parsedRequest.location = parsedRequest.pathInfo + "/";
		}
	}
}

void    			checkServerConfigBlock(t_request &parsedRequest, Config *serverConfigBlock)
{
	if (!serverConfigBlock)
		parsedRequest.statusCode = "400 Bad Request";
	else
	{
		if ((unsigned long)atoi(parsedRequest.contentLenght.c_str()) > serverConfigBlock->getMaxBodySize())
		{
			parsedRequest.statusCode = "413 Request Entity Too Large";
			return ;
		}
	}
}

const t_location	*checkLocationBlock(t_request &parsedRequest, Config *serverConfigBlock)
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
	}
	return (locationBlock);
}

void    checkingProtocol(t_request &req)
{
    if (req.protocol != "HTTP/1.1")
    {
        req.statusCode = "400 Bad Request";
        std::cerr << T_YB "Wrong method" T_N << std::endl;
    }
    else
        req.statusCode = "200 OK";
}

void    checkingMethod(t_request &req, const std::vector<std::string> &method)
{
    if( std::find(method.begin(), method.end(), req.requestMethod) == method.end() )
    {
        req.statusCode = "405 Method Not Allowed";
        std::cerr << T_YB "Wrong method" T_N << std::endl;
    }
}