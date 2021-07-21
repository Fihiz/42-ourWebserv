/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckingHeader.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgoudet <pgoudet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/10 09:32:20 by pgoudet           #+#    #+#             */
/*   Updated: 2021/07/21 15:32:40 by pgoudet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Respond.hpp"
#include "../config/Config.hpp"


Respond checkingHeader(t_request *req, const std::vector<std::string> method, Config conf)
{
    struct stat file;
	Respond clientResp;

    if( std::find(method.begin(), method.end(), req->requestMethod) == method.end() )
    {
		clientResp.setStatusCode("405 Method Not Allowed");
        std::cerr << T_YB "Wrong method" T_N << std::endl;
		if (stat(conf.getErrorPage().find(405)->second.c_str(), &file) == 0)
		{
			clientResp.setPathToError(conf.getErrorPage().find(405)->second);
		}
    }
    else if (req->protocol.compare(0, 8, "HTTP/1.1") != 0)
	{
		clientResp.setStatusCode("400 Bad Request");
        std::cerr << T_YB "Wrong protocol HTTP/1.1" T_N << std::endl;
		if (stat(conf.getErrorPage().find(400)->second.c_str(), &file) == 0)
		{
			clientResp.setPathToError("./pages/400.html");
		}
	}
    else if (req->fullPathInfo.empty() == true || (stat((req->fullPathInfo).c_str(), &file)) != 0)
    {
		clientResp.setStatusCode("404 Not Found");
        std::cerr << T_YB "Wrong file path" T_N << std::endl;
		if (stat(conf.getErrorPage().find(404)->second.c_str(), &file) == 0)
		{
			clientResp.setPathToError(conf.getErrorPage().find(404)->second.c_str());
		}
    }
	return (clientResp);
}