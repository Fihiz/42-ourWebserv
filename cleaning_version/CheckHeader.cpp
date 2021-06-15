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

    if (req.request_method.empty() == false && req.request_method.compare("GET") != 0 
        && req.request_method.compare("POST") != 0
        && req.request_method.compare("DELETE") != 0)
    {
        std::cerr << "Wrong method" << std::endl;
        return(-1);
    }
    else if (req.protocol.compare(0, 8, "HTTP/1.1") != 0)
	{
        std::cerr << "Wrong protocol HTTP/1.1" << std::endl;
        return (-1);
	}
    else if (req.path_info.empty() == true || (stat((req.path_info).c_str(), &file) != 0))
    {
        std::cerr << "Wrong file path" << std::endl;
        return (-1);
    }
    return (0);
}