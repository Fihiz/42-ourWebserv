/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgoudet <pgoudet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/10 16:10:29 by sad-aude          #+#    #+#             */
/*   Updated: 2021/06/15 15:47:23 by pgoudet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <unistd.h>
#include <iostream>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdexcept> // To do: try and catch
#include <time.h>
#include <vector>
#include "All.hpp"
#include "Request.hpp"

# define CLEAN "\e[1;1H\e[2J"
# define T_N "\033[00m"
# define T_YB "\033[01;33m"
# define T_GNB "\033[01;32m"
# define T_GYB "\033[01;90m"
# define T_BB "\033[01;34m"
# define T_CB "\033[01;36m"
# define T_RB "\033[01;31m"

/* PAGE CREATION */

std::string     getFileContent( std::string fullFileName );
void            getFileInfo( char *request, std::string &fileName, std::string &ext, std::string &contentType );
std::string     getDateFormat( time_t &date );
std::string     numFormat( int nb );
std::string     createAutoIndex( std::string &fileName );


/* CLOSE AND ERROR MANAGEMENT */

class Socket; // Need to deal with it later

void            losingConnexion( int fd, fd_set &read_set, std::string const type );
int				error( std::string str, fd_set &read_set, Socket &master );
void			closeAllFdUnlessMaster( fd_set &read_set, Socket &master );

#endif