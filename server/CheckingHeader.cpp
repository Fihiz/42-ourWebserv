/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckingHeader.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/10 09:32:20 by pgoudet           #+#    #+#             */
/*   Updated: 2021/06/30 14:36:43 by sad-aude         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

void checkingHeader(t_request *req)
{
    struct stat file;

    req->statusCode = "200 OK";
    if (req->requestMethod.empty() == true || (req->requestMethod.compare("GET") != 0 
        && req->requestMethod.compare("POST") != 0
        && req->requestMethod.compare("DELETE") != 0))
    {
        req->statusCode = "405 Method Not Allowed";
        std::cerr << T_YB "Wrong method" T_N << std::endl;
        req->pathInfo = "./pages/405.html"; // Add stat to check if the file is currently existant
    }
    else if (req->protocol.compare(0, 8, "HTTP/1.1") != 0)
	{
        req->statusCode = "400 Bad Request";
        std::cerr << T_YB "Wrong protocol HTTP/1.1" T_N << std::endl;
        req->pathInfo = "./pages/400.html";
	}
    else if (req->pathInfo.empty() == true || (stat((req->pathInfo).c_str(), &file)) != 0)
    {
        req->statusCode = "404 Not Found";
        std::cerr << T_YB "Wrong file path" T_N << std::endl;
        req->pathInfo = "./pages/404.html";
    }
}