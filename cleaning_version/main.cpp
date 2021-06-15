/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgoudet <pgoudet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 11:59:24 by sad-aude          #+#    #+#             */
/*   Updated: 2021/06/15 17:07:59 by pgoudet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

void    processMasterSocket( fd_set &read_set, Socket &master )
{
    int     clientSock;

    clientSock = accept(master.getMasterSock(), NULL, NULL);
    if (clientSock < 0)
        error("Accept connexion", read_set, master);
    FD_SET(clientSock, &read_set);
    std::cout << T_BB "New connexion established on [" << clientSock << "]" T_N << std::endl;
    return ;
}

int     processSockets( int fd, fd_set &read_set, Socket &master, char **envp)
{
    char    request[20000];
    int     running = 1;

	if (fd == master.getMasterSock())
        processMasterSocket(read_set, master);
    else
    {
        ssize_t ret = recv(fd, request, 19999, 0); // Flags to check later
        if (ret < 0)
            losingConnexion( fd, read_set, "Connexion lost... (");
        else
        {
            request[ret] = '\0';
            std::string fileName;
            std::string ext;
            std::string contentType;
            std::string content;
            struct stat buf;
			t_request	req;
			int			check;
			t_serv serv;
			
	  		serv.server_name = reinterpret_cast<const char *>("localhost");
    		serv.server_port = reinterpret_cast<const char *>("8080");
    		serv.server_protocol = reinterpret_cast<const char *>("HTTP/1.1");
            //getFileInfo(request, fileName, ext, contentType);
			req = takeInfo(request);
			std::cout \
			<< "req.User-Agent=" << req.user_agent \
			<< "\naccepted_charset=" << req.accepted_charset \
			<< "\naccepted_language=" << req.accepted_language \
			<< "\nallow=" << req.allow \
			<< "\nauthorization=" << req.authorization \
			<< "\ncontent_language=" << req.content_language \
			<< "\ncontent_lenght=" << req.content_lenght \
			<< "\ncontent_location=" <<req.content_location \
			<< "\ncontent_type=" << req.content_type \
			<< "\ndate=" << req.date \
			<< "\nhost=" << req.host \
			<< "\nlast_modified=" << req.last_modified \
			<< "\nlocation=" << req.location \
			<< "\nreferer=" << req.referer \
			<< "\nretry_after=" << req.retry_after \
			<< "\nserver=" << req.server \
			<< "\ntransfert_encoding=" << req.transfert_encoding \
			<< "\nuser_agent=" << req.user_agent \
			<< "\nwww_authenticate=" << req.www_authenticate \
			<< "\nprotocol=" << req.protocol \
			<< "\npath_info=" << req.path_info \
			<< "\nrequest_method=" << req.request_method \
			<< std::endl;
			std::cout << req.content << std::endl;
			check = checkHeader(req);
			// if (check != 0)
  			// {
			// 	std::cout << check << std::endl;
  			//     write(1, "wrong file path\n", strlen("wrong file path\n"));
			// 	losingConnexion( fd, read_set, "CKC... (");
  			//     return (-1);
  			// }
			if ((envp = initEnvp(envp, req, serv)) == NULL)
    		{
    		    write(1, "bad request\n", strlen("bad request\n"));
    		    return (1);
    		}
			std::string::size_type dot = (req.path_info).find('.');
			if (dot != std::string::npos)
			{
				fileName = (req.path_info).substr(0, dot);
				ext = (req.path_info).substr(dot);
			}
			else
			{
				fileName = (req.path_info);
				ext = "";
			}
			fileName = "." + fileName;
			// for (int i = 0; i < tab_size(envp); i++)
   			//     std::cout << envp[i] << std::endl;
			if (ext != "")
			{
				stat((fileName + "." + ext).c_str(), &buf);
				if (!S_ISDIR(buf.st_mode))
					content = getFileContent((fileName + "." + ext));
			}
			else
			{
				stat(fileName.c_str(), &buf);
				if (!S_ISDIR(buf.st_mode))
					content = getFileContent(fileName);
			}
			if (S_ISDIR(buf.st_mode))
			{
                content = createAutoIndex(fileName);
                if (!content.size()) // if null, return 404
                    return (EXIT_FAILURE);
			}
            std::string responseHeader = "HTTP/1.1 200 OK\nContent-Type:" + contentType + "\nContent-Length:" + std::to_string(content.size()) + "\n\n" + content;
            if ((std::string)request == "exit\n") // (?)
                running = 0;
			std::cout << fd << " is requesting :" << std::endl << request << std::endl;
            // std::cout << T_YB << responseHeader << T_N << std::endl;
            if (send(fd, responseHeader.c_str(), responseHeader.size(), 0) < 0)
                error("Send", read_set, master);
            losingConnexion( fd, read_set, "Closing... (");
        }
    }
    return (running);
}

int     main( int ac, char *av[], char *envp[] )
{
    Socket master( 8080 );

    fd_set read_set;
    fd_set read_copy;

    FD_ZERO( &read_set );
    FD_ZERO( &read_copy );
    FD_SET( master.getMasterSock(), &read_set );

    int running = 1;
    int fd = 0;
	(void)ac;
	(void)av;
    while (running)
    {
        std::cout << T_GYB "Waiting in passive mode" T_N << std::endl;
        read_copy = read_set;
        if (select(FD_SETSIZE, &read_copy, 0, 0, 0) < 0)
            error("Select", read_set, master);
        for (fd = 0; fd <= FD_SETSIZE; ++fd)
        {
            if (FD_ISSET(fd, &read_copy))
            {
                running = processSockets(fd, read_set, master, envp);
                break ;
            }
        }
    }
    closeAllFdUnlessMaster(read_set, master); // Destructor destroys the master
    return (0);
}