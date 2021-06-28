/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobenass <jobenass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 11:21:35 by sad-aude          #+#    #+#             */
/*   Updated: 2021/06/23 15:25:55 by jobenass         ###   ########lyon.fr   */
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
		struct sockaddr_in _servAddr;


		Socket( Socket const &src );
		Socket &operator=( Socket const &rhs );
	

	public:
		Socket( void );
		// Socket(Config const & it) ;
		Socket( int port );
		~Socket( void );

		int			getMasterSock( void ) const;
		
		void		doSocket( void );
		void		doBind( void );
		void		doListen( void );
};

#endif