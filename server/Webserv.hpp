/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/10 16:10:29 by sad-aude          #+#    #+#             */
/*   Updated: 2021/07/30 17:51:35 by sad-aude         ###   ########lyon.fr   */
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
#include <stdexcept>
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

/* MAIN */
std::vector<Config>	configuration(int argc, char **argv);
void    			serverLoop(WebservData &Data);

/* PROCESS SOCKET */
int     			processClientSocket(WebservData &Data, int fd);
void    			processMasterSocket(WebservData &Data, int fd);
int     			processSockets(int fd, WebservData &Data);

/* REQUEST MANAGER */
t_request       	parsingRequest( std::string buffer );
void    			setFullPathInfo(t_request &parsedRequest, Config &serverConfigBlock, std::string &tmpFile);
const t_location 	*findLocationBlock(Config &serverConfigBlock, t_request &parsedRequest);
Config 				*findServerConfigBlock(WebservData &Data, std::string host);
off_t     			getFdSize(int fd);
ssize_t     		receiveClientRequest(int fd, std::string &clientRequest);

/* REQUEST CHECKER */
void    			checkRedir(Config *serverConfigBlock, t_request &parsedRequest);
void    			checkServerConfigBlock(t_request &parsedRequest, Config *serverConfigBlock);
const t_location	*checkLocationBlock(t_request &parsedRequest, Config *serverConfigBlock);
void    			checkingProtocol(t_request &req);
void    			checkingMethod(t_request &req, const std::vector<std::string> &method);

/* PAGE CREATION */

std::string     	getFileContent( std::string fullFileName );
void            	getFileInfo( char *request, std::string &fileName, std::string &ext, std::string &contentType );
std::string     	getDateFormat( time_t &date );
std::string     	numFormat( int nb );
std::string     	createAutoIndex( std::string &fullFileName, std::string &fileName );
void            	setContentDependingOnFileOrDirectory(t_request &parsedRequest, const t_location *loc, Config * conf);
std::string     	getContentFileError(Config * virtualHost, std::string causeError);
std::string     	createAutoIndex( std::string &fullFileName, std::string &fileName );

/* RESPONSE MANAGER */
void				printOutputs(int fd, t_request	parsedRequest, std::string clientRequest,std::string responseToClient);
void            	redirectCgiOutputToClient(t_request &req, Config *serverConfigBlock);
std::string    		buildClientResponse(t_request &parsedRequest, const t_location *locationBlock, Config *serverConfigBlock);
void				sendResponseToClient(int fd, WebservData &Data, std::string &responseToClient);
int        			checkPath(t_request &parsedRequest);

/* FD MANAGER */
void            	destroyTabMaster(std::vector<Socket *> tabMaster);
int            		isTabMaster(std::vector<Socket *> tabMaster, int ind);
void           		losingConnexion( int fd, fd_set &readSet, fd_set &writeSet, std::string const type );
int					error( std::string str, WebservData &Data);
void				closeAllFdUnlessMaster( fd_set &readSet, fd_set &writeSet, std::vector<Socket *> tabMaster );

#include "./WebservData.hpp"
#include "./Socket.hpp"

#endif
