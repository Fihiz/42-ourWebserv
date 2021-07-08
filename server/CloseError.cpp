/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CloseError.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agathe <agathe@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/15 13:26:10 by sad-aude          #+#    #+#             */
/*   Updated: 2021/07/08 13:50:01 by agathe           ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "./WebservData.hpp"

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


void    closeAllFdUnlessMaster(fd_set &readSet, std::vector<Socket *> tabMaster)
{
    (void) readSet;
    (void) tabMaster;
    for (int ind = 0; ind <= FD_SETSIZE; ++ind)
    {
        // if (/*FD_ISSET(ind, &readSet) && (isTabMaster(tabMaster, ind) == 0) */0)
        if (FD_ISSET(ind, &readSet) && (isTabMaster(tabMaster, ind) == 0))
        {
            std::cerr << T_YB << "Connection lost... fd=" << ind << T_N << std::endl;
            close(ind);
        }
    }
    return ;
}

int     error(std::string str, WebservData &Data)
{
    perror(str.c_str());
    closeAllFdUnlessMaster(Data.getReadSet(), Data.getTabMaster());
    destroyTabMaster(Data.getTabMaster());
    exit(EXIT_FAILURE);
}

void    losingConnexion(int fd, fd_set &readSet, std::string const type)
{
    std::cerr << T_YB << type << "fd=" << fd << "]" T_N << std::endl;
    FD_CLR(fd, &readSet);
    close(fd);
    return ;
}