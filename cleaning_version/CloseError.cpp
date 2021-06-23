/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CloseError.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobenass <jobenass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/15 13:26:10 by sad-aude          #+#    #+#             */
/*   Updated: 2021/06/23 16:34:09 by jobenass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

void destroyTabMaster(std::vector<Socket *> tabMaster)
{
    for (std::vector<Socket *>::iterator it = tabMaster.begin(); it != tabMaster.end(); ++it) {
        delete (*it);
    }
}


int isTabMaster(std::vector<Socket *> tabMaster, int ind)
{
    for (std::vector<Socket *>::iterator it = tabMaster.begin(); it != tabMaster.end(); ++it) {
        if (ind == (*it)->getMasterSock())
        {
            return(1);
        }
    }

    return (0);
}


void    closeAllFdUnlessMaster(fd_set &read_set, std::vector<Socket *> tabMaster)
{
    for (int ind = 0; ind <= FD_SETSIZE; ++ind)
    {
        if (FD_ISSET(ind, &read_set) && (isTabMaster(tabMaster, ind) == 0) )
        {
            std::cerr << T_YB << "Connection lost... fd=" << ind << T_N << std::endl;
            close(ind);
        }
    }
    return ;
}

int     error(std::string str, fd_set &read_set, std::vector<Socket *> tabMaster)
{
    perror(str.c_str());
    closeAllFdUnlessMaster(read_set, tabMaster);
    destroyTabMaster(tabMaster);
    exit(EXIT_FAILURE);
}

void    losingConnexion(int fd, fd_set &read_set, std::string const type)
{
    std::cerr << T_YB << type << "fd=" << fd << "]" T_N << std::endl;
    FD_CLR(fd, &read_set);
    close(fd);
    return ;
}