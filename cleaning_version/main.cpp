/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 11:59:24 by sad-aude          #+#    #+#             */
/*   Updated: 2021/06/17 18:37:40 by sad-aude         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

void    processMasterSocket(fd_set &read_set, Socket &master)
{
    int     clientSock;

    clientSock = accept(master.getMasterSock(), NULL, NULL);
    if (clientSock < 0)
        error("Accept connexion", read_set, master);
    FD_SET(clientSock, &read_set);
    std::cout << T_BB "New connexion established on [" << clientSock << "]" T_N << std::endl;
    return ;
}

int     processSockets(int fd, fd_set &read_set, Socket &master, char **env)
{
    char    requestBuffer[20000];
    int     running = 1;

	if (fd == master.getMasterSock())
        processMasterSocket(read_set, master);
    else
    {
        ssize_t len = recv(fd, requestBuffer, 19999, 0); // Flags to check later
        if (len < 0)
            losingConnexion(fd, read_set, "Connexion lost... (");
        else
        {
            requestBuffer[len] = '\0';
			t_request	parsedRequest;
            
            /* For CGI, not needed for the moment (?) */
			//t_serv      serv;
            /* ----------------------------------------- */
            
            (void) env;

            /* For CGI, not needed for the moment (?) */
	  		// serv.server_name = reinterpret_cast<const char *>("localhost");
    		// serv.server_port = reinterpret_cast<const char *>("8080");
    		// serv.server_protocol = reinterpret_cast<const char *>("HTTP/1.1");
            /* ----------------------------------------- */

			parsedRequest = parsingRequest(requestBuffer);
			if (checkingHeader(&parsedRequest))
  			{
				losingConnexion(fd, read_set, "Losing connexion: header is corrupted... (");
  			    return (-1);
  			}

            /* For CGI, not needed for the moment (?) */
			// if ((env = initEnv(env, req, serv)) == NULL)
    		// {
    		//     losingConnexion(fd, read_set, "Losing connexion: environmnent is corrupted... (");
    		//     return (-1);
    		// }
            /* ----------------------------------------- */
            
            setContentDependingOnFileOrDirectory(parsedRequest);
            std::string responseToClient = "HTTP/1.1 200 OK\nContent-Type:" + parsedRequest.fileType + "\nContent-Length:" 
                                        + std::to_string(parsedRequest.fileContent.size()) + "\n\n" + parsedRequest.fileContent;
            if (parsedRequest.pathInfo == "./pages/exit.html") // (?)
                running = 0;
			std::cout << T_CB << fd << " is requesting :" << T_N  << std::endl << requestBuffer << std::endl;
            // std::cout << T_YB << responseToClient << T_N << std::endl;
            if (send(fd, responseToClient.c_str(), responseToClient.size(), 0) < 0)
                error("Send", read_set, master);
            losingConnexion( fd, read_set, "Closing... (");
        }
    }
    return (running);
}

int     main(int ac, char *av[], char *env[])
{
    Socket master( 8080 );

    fd_set read_set;
    fd_set read_copy;

    FD_ZERO(&read_set);
    FD_ZERO(&read_copy);
    FD_SET(master.getMasterSock(), &read_set);

    int running = 1;
	(void)ac;
	(void)av;

    while (running)
    {
        std::cout << T_GYB "Waiting in passive mode" T_N << std::endl;
        read_copy = read_set;
        if (select(FD_SETSIZE, &read_copy, 0, 0, 0) < 0)
            error("Select", read_set, master);
        for (int fd = 0; fd <= FD_SETSIZE; ++fd)
        {
            if (FD_ISSET(fd, &read_copy))
            {
                running = processSockets(fd, read_set, master, env);
                break ;
            }
        }
    }
    closeAllFdUnlessMaster(read_set, master); // Destructor destroys the master
    return (0);
}