/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   All.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 08:26:50 by pgoudet           #+#    #+#             */
/*   Updated: 2021/06/17 15:43:50 by sad-aude         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef ALL_HPP
#define ALL_HPP

#include "Request.hpp"
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <sys/stat.h>
#include <sstream>
#include <iostream>
#include <sys/select.h>
#include <sys/time.h>

typedef struct s_serv
{
   const char *server_name;
   const char *server_protocol;
   const char *server_port;
}               t_serv;

int getAnswer(t_request const &req);
int postAnswer(t_request const &req);
int deleteAnswer(t_request const &req);
char **initEnvp(char **env, t_request const &req, t_serv serv);
int tabSize(char **tab);
t_request  parsingRequest(char *buffer);
int checkingHeader(t_request req);


#endif