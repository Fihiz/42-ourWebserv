/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobenass <jobenass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 11:21:35 by sad-aude          #+#    #+#             */
/*   Updated: 2021/06/22 15:08:59 by jobenass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

#include "Webserv.hpp"
#include "./config/Config.hpp"

class Socket
{
	private:
		int		_masterSock;
		struct sockaddr_in _servAddr;


		Socket( void );
		Socket( Socket const &src );
		Socket &operator=( Socket const &rhs );
	

	public:
		// Socket(Config const & it) ;
		Socket( std::vector<Config>::iterator it );
		~Socket( void );

		int			getMasterSock( void ) const;
		
		void		doSocket( void );
		void		doBind( void );
		void		doListen( void );
};

#endif