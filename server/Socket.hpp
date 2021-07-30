/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 11:21:35 by sad-aude          #+#    #+#             */
/*   Updated: 2021/07/30 17:37:27 by sad-aude         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

#include "Webserv.hpp"
#include "../config/Config.hpp"

class Socket
{
	private:
		int		_masterSock;
		int		_usedPort;
		struct sockaddr_in _servAddr;

		Socket( Socket const &src );
		Socket &operator=( Socket const &rhs );

	public:
		Socket( void );
		Socket( int port );
		~Socket( void );

		int			getMasterSock( void ) const;
		int			getUsedPort( void ) const;

		void		doSocket( void );
		void		doBind( void );
		void		doListen( void );
};

#endif
