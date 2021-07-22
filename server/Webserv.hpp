/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agathe <agathe@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/10 16:10:29 by sad-aude          #+#    #+#             */
/*   Updated: 2021/07/21 22:35:17 by agathe           ###   ########lyon.fr   */
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
#include <fcntl.h>
#include "Request.hpp"
#include "../config/Config.hpp"

# define CLEAN "\e[1;1H\e[2J"
# define T_N "\033[00m"
# define T_YB "\033[01;33m"
# define T_GNB "\033[01;32m"
# define T_GYB "\033[01;90m"
# define T_BB "\033[01;34m"
# define T_CB "\033[01;36m"
# define T_RB "\033[01;31m"

class Socket;
class WebservData;

typedef struct s_serv
{
   const char *server_name;
   const char *server_protocol;
   const char *server_port;
}               t_serv;


/* PAGE CREATION */

std::string     getFileContent( std::string fullFileName );
void            getFileInfo( char *request, std::string &fileName, std::string &ext, std::string &contentType );
std::string     getDateFormat( time_t &date );
std::string     numFormat( int nb );
std::string     createAutoIndex( std::string &fullFileName, std::string &fileName );
void            setContentDependingOnFileOrDirectory( t_request &parsedRequest, const t_location *loc );


/* CLOSE AND ERROR MANAGEMENT */
void           destroyTabMaster(std::vector<Socket *> tabMaster);
int            isTabMaster(std::vector<Socket *> tabMaster, int ind);

class Socket; // Need to deal with it later

void           losingConnexion( int fd, fd_set &readSet, std::string const type );
int				error( std::string str, WebservData &Data);
void			   closeAllFdUnlessMaster( fd_set &readSet, std::vector<Socket *> tabMaster );

/* REQUEST */

int             getAnswer( t_request const &req );
int             postAnswer( t_request const &req );
int             deleteAnswer( t_request const &req );
char **         initEnv( char **env, t_request const &req, t_serv serv );
int             tabSize( char **tab );
t_request       parsingRequest( std::string buffer );
// ORIGINAL
// t_request       parsingRequest( char *buffer );
void            checkingHeader( t_request *req, const std::vector<std::string> method );

#include "./WebservData.hpp"
#include "./Socket.hpp"

#endif