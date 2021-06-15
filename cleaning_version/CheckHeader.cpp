/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checkHeader.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgoudet <pgoudet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/10 09:32:20 by pgoudet           #+#    #+#             */
/*   Updated: 2021/06/15 17:02:59 by pgoudet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "All.hpp"

int checkHeader(t_request req)
{
    struct stat file;
    int test;

    if (req.request_method.empty() == false && req.request_method.compare("GET") != 0 
        && req.request_method.compare("POST") != 0
        && req.request_method.compare("DELETE") != 0)
        return(-1);
    else if (req.protocol.compare(0, 8, "HTTP/1.1") != 0)
	{
        return (-2);
	}
    else if (req.path_info.empty() == false && (test = stat(req.path_info.c_str(), &file) != 0))
        return (-3);
    return (0);
}