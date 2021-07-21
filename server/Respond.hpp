/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Respond.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgoudet <pgoudet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/20 14:02:36 by pgoudet           #+#    #+#             */
/*   Updated: 2021/07/21 14:58:52 by pgoudet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPOND_HPP
#define RESPOND_HPP
#include "../config/Config.hpp"
#include "WebservData.hpp"
#include "Webserv.hpp"
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

class Respond
{
	private:
		std::string _contentType;
		std::string _contentLenght;
		int 		_statusCode;
		std::string _statusMessage;
		std::string _fileContent;
		std::string _finalResp;
		std::string _pathToError;
	public:
		Respond();
		~Respond();
		Respond(const Respond &Resp);
		
		void	operator=(const Respond &Resp);
		void checkingHeader(t_request *req, const std::vector<std::string> method);

		void		setFd(int fd);
		void		setContentLenght(std::string lenght);
		void		setContentType(std::string type);
		void		setStatusCode(std::string code);
		void		setFileContent(std::string const &path);
		void		setPathToError(std::string path);
		std::string getPathToError(void) const;
		std::string	getStatusMessage() const;
		std::string	getContentLenght() const;
		std::string	getContentType() const;
		int			getStatusCode() const;
		std::string getFileContent() const;
		int			finalRespond(int fd, t_request &parsedRequest, char **env, WebservData &Data);
};

std::ofstream	&operator<<(std::ofstream &ofs, const Respond &resp);

#endif
