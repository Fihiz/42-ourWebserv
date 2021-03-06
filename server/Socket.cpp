/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 11:21:41 by sad-aude          #+#    #+#             */
/*   Updated: 2021/07/30 17:37:06 by sad-aude         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Socket::Socket( void )
{
    std::cout << T_GYB "Socket constructor called" T_N << std::endl;
    return ;
}

Socket::Socket(int port)
: _usedPort(port)
{
    this->doSocket();

    this->_servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    this->_servAddr.sin_family = AF_INET;
    this->_servAddr.sin_port = htons(port);

    this->doBind();
    this->doListen();
    return ;
}

Socket::~Socket( void )
{
    std::cout << T_GYB "Socket destructor called" T_N << std::endl;
    close(this->_masterSock);
    return ;
}

int     Socket::getMasterSock( void ) const
{
    return (this->_masterSock);
}

int     Socket::getUsedPort( void ) const
{
    return (this->_usedPort);
}

void    Socket::doListen( void )
{
    if (listen(this->_masterSock, 128) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
    return ;
}

void    Socket::doSocket( void )
{
    if ((this->_masterSock = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket");
        exit(EXIT_FAILURE);
	}
    std::cout << T_BB "Using fd [" T_GNB << this->_masterSock << T_BB "] for listening on port [" T_GNB << getUsedPort() << T_BB "]" << T_N << std::endl;
    
    int opt = 1;
    if (setsockopt(this->_masterSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
}

void    Socket::doBind( void )
{
    if (bind(this->_masterSock, (sockaddr *)&(this->_servAddr), sizeof(this->_servAddr)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    return ;
}
