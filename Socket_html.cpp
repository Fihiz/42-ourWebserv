/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket_html.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 07:06:31 by salome            #+#    #+#             */
/*   Updated: 2021/06/08 10:59:54 by sad-aude         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

# define CLEAN "\e[1;1H\e[2J"
# define T_N "\033[00m"
# define T_YB "\033[2;33m"
# define T_GNB "\033[01;32m"
# define T_GYB "\033[01;90m"
# define T_BB "\033[01;34m"
# define T_CB "\033[01;36m"
# define T_RB "\033[01;31m"

int main(int ac, char **av)
{

	std::cout << CLEAN << std::endl;

    int listen_sock;
    int accept_sock;
    int ret;
    int opt = 1;
    int running = 1;
	struct sockaddr_in sin;
	int recsize = sizeof(sockaddr);
    
    std::ifstream	ifs("index.html");
    std::stringstream   line;
    line << ifs.rdbuf();
    std::string content = line.str();
    std::string header = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:" + std::to_string(content.size()) + "\n\n" + content;

    char    request[20000];
   
    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket");
        exit(EXIT_FAILURE);
	}
    std::cout << T_BB "Using fd [" <<listen_sock << "] for listening" T_N << std::endl;

    if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    inet_aton("127.0.0.1", (&sin.sin_addr)); // To convert the host address (?)
    // sin.sin_addr.s_addr = htonl(INADDR_ANY);
    //std::cout << sin.sin_addr.s_addr << std::endl;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8080);

    if (bind(listen_sock, (sockaddr *)&sin, sizeof(sin)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

	if (listen(listen_sock, 5) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
    
	while (running)
    {
        std::cout << T_GYB "Waiting in passive mode" T_N << std::endl;
        accept_sock = accept(listen_sock, (sockaddr *)&sin, (socklen_t *)&recsize);
        std::cout << T_BB "Using fd [" << accept_sock << "] for connexion" << std::endl;
        //std::cout << "on a accepté : " << sin.sin_family << " " << sin.sin_port << " " << sin.sin_addr.s_addr << std::endl;
        
		ret = read(accept_sock, request, 19999);
        if (ret < 0)
		{
            //std::cout << T_YB "Something went wrong for read" T_N<< std::endl;
			perror("read");
			exit(EXIT_FAILURE);
		}
        else
        {
            request[ret] = '\0';
            std::cout << T_GNB "On sock: [" << accept_sock << "]" << " port: [" << sin.sin_port << "], request is " << std::endl << std::endl << "{" T_N << request << T_GNB "}" T_N << std::endl;
            if ((std::string)request == "exit\n") // (?)
                running = 0;
            // std::cout << header << std::endl;
            send(accept_sock, header.c_str(), header.size(), 0);
            // write(sock2, header, ((std::string)header).size());
            close(accept_sock);
        }
    }
    close(listen_sock);
    return (EXIT_SUCCESS);
}

// Remember errno is normally forbidden