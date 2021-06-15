/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initEnvp.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgoudet <pgoudet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 14:07:57 by pgoudet           #+#    #+#             */
/*   Updated: 2021/06/15 16:07:40 by pgoudet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "All.hpp"

void   error_failure(std::string str)
{
     std::cout << str;
     exit(1);
}

int tab_size(char **tab)
{
    int i = 0;

    while(tab[i])
        i++;
    return (i);
}

int    cp_tab(char **dest, char **src)
{
    int i = 0;

    while (src[i])
    {
        dest[i] = strdup(src[i]);
        i++;
    }
    dest[i] = NULL;
    return (i);
}

void    associate(char **str, std::string content, std::string src, int *i)
{
    if (content.empty() == false && src.empty() == false)
    {
        *str = strdup((char *)(content + src).c_str());
        (*i)++;
    }
}

char **initEnvp(char **envp, t_request const &req, t_serv serv)
{
    char **var;
    int i, tmp;

    var = (char **)malloc(sizeof(void *) * (17 + tab_size(envp)));
    if((i = cp_tab(var, envp)) == -1)
        error_failure("init_envp\n");
    tmp = i;
    associate(&var[i], "AUTH_TYPE=", req.content_lenght, &i);
    associate(&var[i], "CONTENT_TYPE=", req.content_type, &i);
    associate(&var[i], "PATH_INFO=", req.path_info, &i);
    associate(&var[i], "PATH_TRANSLATED=", req.path_translated, &i);
    associate(&var[i], "QUERY_STRING=", req.query_string, &i);
    associate(&var[i], "REMOTE_ADDR=", req.remote_addr, &i);
    associate(&var[i], "REMOTE_USER=", req.remote_addr, &i);
    associate(&var[i], "REQUEST_METHOD=", req.request_method, &i);
    associate(&var[i], "REQUEST_URI=", req.uri, &i);
    associate(&var[i], "SCRIPT_NAME=", req.script, &i);
    associate(&var[i], "SERVER_NAME=", serv.server_name, &i);
    associate(&var[i], "SERVER_PORT=", serv.server_port, &i);
    associate(&var[i], "SERVER_PROTOCOL=", serv.server_protocol, &i);
    var[i] = NULL;
    if (i == tmp)
        return (NULL);
    return (var);
}