/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 08:08:07 by pgoudet           #+#    #+#             */
/*   Updated: 2021/06/17 18:38:48 by sad-aude         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>

typedef struct s_request
{
    std::string acceptedCharset; //+
    std::string acceptedLanguage; //+
    std::string allow; //+
    std::string authorization; //+
    std::string contentLanguage; //+
    std::string contentLenght; //+
    std::string contentLocation; //+
    std::string contentType; //+
    std::string date; //+
    std::string host; // +
    std::string lastModified; //+
    std::string location; //+
    std::string referer; //+
    std::string retryAfter; //+
    std::string server; // +
    std::string transfertEncoding; //+
    std::string userAgent; //+
    std::string wwwAuthenticate; //+

    std::string body; //+
    std::string protocol; // +
    std::string pathInfo; //+
    std::string requestMethod; // +
    std::string pathTranslated; // +
    std::string script;   // -
    std::string queryString; // -
    std::string uri; // -
    std::string remoteUser; //-
    std::string remoteAddr; // -
    std::string auth_type; //-
    // Added by socket team
    std::string fileContent;
    std::string fileType;
    std::string statusCode;
    //std::string fileContent; (?)
    

}               t_request;

#include "Webserv.hpp"

#endif