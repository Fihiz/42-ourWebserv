/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Respond.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgoudet <pgoudet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/20 14:19:13 by pgoudet           #+#    #+#             */
/*   Updated: 2021/07/21 16:39:56 by pgoudet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Respond.hpp"

Respond::Respond(): _statusCode(200), _statusMessage("Ok"){}

Respond::~Respond(){}

Respond::Respond(const Respond &Resp) {*this = Resp;}
		
void	Respond::operator=(const Respond &Resp)
{
	this->_contentType = Resp._contentType;
	this->_contentLenght = Resp._contentLenght;
	this->_statusCode = Resp._statusCode;
}

std::ofstream	&operator<<(std::ofstream &ofs, const Respond &resp)
{
	ofs <<  "HTTP/1.1 " << resp.getStatusCode() << "\nContent-Type:" << resp.getContentType() << "\nContent-Length:" << resp.getContentLenght() << "\n";
	return (ofs);
}

void		Respond::setContentLenght(std::string lenght){ this->_contentLenght = lenght; }

void		Respond::setContentType(std::string type){ this->_contentType = type; }

void		Respond::setStatusCode(std::string code)
{
	if (this->_statusCode < 300)
		this->_statusCode = atoi(code.c_str()); this->_statusMessage = &code[3];
}

void		Respond::setFileContent(std::string const &path)
{
	std::ifstream ifs(path);
	std::stringstream   line;
	if (ifs.is_open() == true)
	{	
   		line << ifs.rdbuf();
   		this->_fileContent = line.str();
   		ifs.close();
	}
}

void		Respond::setPathToError(std::string path) { this->_pathToError = path; }

std::string Respond::getPathToError(void) const { return (this->_pathToError); }

std::string	Respond::getContentLenght() const { return (this->_contentLenght); }

std::string	Respond::getContentType() const { return (this->_contentType); }

int			Respond::getStatusCode() const { return (this->_statusCode); }

std::string Respond::getFileContent() const { return (this->_fileContent); }

std::string	Respond::getStatusMessage() const { return (this->_statusMessage); }

int			Respond::finalRespond(int fd, t_request &parsedRequest, char **env, WebservData &Data)
{
	int running = 1;
	// parsedRequest.pathInfoCgi = "../cgi-bin/php-cgi";
	if (parsedRequest.fileExt == "php" && parsedRequest.pathInfoCgi.empty() == false)
	{
		redirectCgiOutputToClient(env, fd, parsedRequest, *this, Data);
	}
	else
	{
		setContentDependingOnFileOrDirectory(parsedRequest);
		this->_finalResp = "HTTP/1.1 " + std::to_string(this->getStatusCode()) + this->getStatusMessage() + "\nContent-Type:" + parsedRequest.fileType + "\nContent-Length:" 
	                            + std::to_string(parsedRequest.fileContent.size()) + "\n\n" + parsedRequest.fileContent;
		if (parsedRequest.pathInfo == "./exit.html")
	 	   running = 0;
		std::cout << T_GYB "Current status code [" << parsedRequest.statusCode << "]" << T_N << std::endl;
		fcntl(fd, F_SETFL, O_NONBLOCK);
		if (send(fd, this->_finalResp.c_str(), this->_finalResp.size(), 0) < 0)
		    error("Send", Data);
		losingConnexion( fd, Data.getReadSet(), "Closing... [");
	}
	return (running);
}