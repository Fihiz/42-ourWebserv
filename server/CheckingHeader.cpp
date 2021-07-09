/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckingHeader.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agathe <agathe@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/10 09:32:20 by pgoudet           #+#    #+#             */
/*   Updated: 2021/07/09 18:07:45 by agathe           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"


void checkingHeader(t_request *req, const std::vector<std::string> method)
{
    struct stat file;

    req->statusCode = "200 OK";
    // if (req->requestMethod.empty() == true || (req->requestMethod.compare("GET") != 0 
    //     && req->requestMethod.compare("POST") != 0
    //     && req->requestMethod.compare("DELETE") != 0))
    // if (((std::vector<std::string>)method).find(req->requestMethod) == method.end())
    
    
    if( std::find(method.begin(), method.end(), req->requestMethod) == method.end() )
    {
        req->statusCode = "405 Method Not Allowed";
        std::cerr << T_YB "Wrong method" T_N << std::endl;
        req->fullPathInfo = "./pages/405.html"; // Add stat to check if the file is currently existant
    }
    else if (req->protocol.compare(0, 8, "HTTP/1.1") != 0)
	{
        req->statusCode = "400 Bad Request";
        std::cerr << T_YB "Wrong protocol HTTP/1.1" T_N << std::endl;
        req->fullPathInfo = "./pages/400.html";
	}
    else if (req->fullPathInfo.empty() == true || (stat((req->fullPathInfo).c_str(), &file)) != 0)
    {
        req->statusCode = "404 Not Found";
        std::cerr << T_YB "Wrong file path" T_N << std::endl;
        req->fullPathInfo = "./pages/404.html";
    }
}