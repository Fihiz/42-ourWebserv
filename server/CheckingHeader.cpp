/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckingHeader.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/10 09:32:20 by pgoudet           #+#    #+#             */
/*   Updated: 2021/06/17 18:37:55 by sad-aude         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

void checkingHeader(t_request *req)
{
    struct stat file;

    req->statusCode = "200 OK";
    if (req->requestMethod.empty() == false && req->requestMethod.compare("GET") != 0 
        && req->requestMethod.compare("POST") != 0
        && req->requestMethod.compare("DELETE") != 0)
    {
        req->statusCode = "405 Method Not Allowed";
        std::cerr << "Wrong method" << std::endl;
        req->pathInfo = "./pages/405.html";
    }
    else if (req->protocol.compare(0, 8, "HTTP/1.1") != 0)
	{
        req->statusCode = "400 Bad Request";
        std::cerr << "Wrong protocol HTTP/1.1" << std::endl;
        req->pathInfo = "./pages/400.html";
	}
    else if (req->pathInfo.empty() == true || (stat((req->pathInfo).c_str(), &file)) != 0)
    {
        req->statusCode = "404 Page not found";
        std::cerr << "Wrong file path" << std::endl;
        req->pathInfo = "./pages/404.html";
    }
}