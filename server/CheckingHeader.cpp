/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckingHeader.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/10 09:32:20 by pgoudet           #+#    #+#             */
/*   Updated: 2021/07/27 19:25:02 by sad-aude         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

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
