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

int checkingHeader(t_request *req)
{
    struct stat file;

    if (req->requestMethod.empty() == false && req->requestMethod.compare("GET") != 0 
        && req->requestMethod.compare("POST") != 0
        && req->requestMethod.compare("DELETE") != 0)
    {
        std::cerr << "Wrong method" << std::endl;
        return(-1);
    }
    else if (req->protocol.compare(0, 8, "HTTP/1.1") != 0)
	{
        std::cerr << "Wrong protocol HTTP/1.1" << std::endl;
        return (-1);
	}
    else if (req->pathInfo.empty() == true || (stat((req->pathInfo).c_str(), &file) != 0))
    {
        std::cerr << "Wrong file path" << std::endl;
        //test ERROR
        req->pathInfo = "./pages/error404Test.html";
        // return (-1);
    }
    return (0);
}