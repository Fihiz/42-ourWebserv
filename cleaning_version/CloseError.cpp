/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CloseError.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/15 13:26:10 by sad-aude          #+#    #+#             */
/*   Updated: 2021/06/15 13:29:07 by sad-aude         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

void    closeAllFdUnlessMaster(fd_set &read_set, Socket &master)
{
    for (int ind = 0; ind <= FD_SETSIZE; ++ind)
    {
        if (FD_ISSET(ind, &read_set) && ind != master.getMasterSock())
        {
            std::cerr << T_YB << "Connection lost... fd=" << ind << T_N << std::endl;
            close(ind);
        }
    }
    return ;
}

int     error(std::string str, fd_set &read_set, Socket &master)
{
    perror(str.c_str());
    closeAllFdUnlessMaster(read_set, master);
    exit(EXIT_FAILURE);
}

void    losingConnexion( int fd, fd_set &read_set, std::string const type )
{
    std::cerr << T_YB << type << "fd=" << fd << ")" T_N << std::endl;
    FD_CLR(fd, &read_set);
    close(fd);
    return ;
}