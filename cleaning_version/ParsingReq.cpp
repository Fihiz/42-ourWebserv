/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsingReq.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgoudet <pgoudet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 16:58:01 by pgoudet           #+#    #+#             */
/*   Updated: 2021/06/15 16:46:40 by pgoudet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "All.hpp"
#include "Request.hpp"


std::string onlyTheGood(std::string str)
{
    std::stringstream ss(str);
    std::string      ss_str;
	char *tmp = const_cast<char *>(str.c_str());
	int i = 0;
    
    std::getline(ss, ss_str);
	while(tmp[i] && tmp[i] != ':')// && tmp[i + 1] != ' ')
		i++;
    if (tmp[i + 1] == ' ')
        i++;
	str = str.substr(i + 1, ss_str.length() - i - 1);
	return (str);
}

void        take_info_cinquies(std::string chain, t_request *req)
{
    int pos = -1;
    std::string value;

    pos = chain.find("Referer", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&chain[pos]);
        req->referer = value;
    }
    pos = chain.find("Retry-After", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&chain[pos]);
        req->retry_after = value;
    }
    pos = chain.find("Server", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&chain[pos]);
        req->server = value;
    }
    pos = chain.find("Transfert-Encoding", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&chain[pos]);
        req->transfert_encoding = value;
    }
}

void       take_info_quater(std::string chain, t_request *req)
{
    int pos = -1;
    std::string value;

    pos = chain.find("Content-Type", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&chain[pos]);
        req->content_type = value;
    }
    pos = chain.find("Host", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&chain[pos]);
        req->host = value;
    }
    pos = chain.find("Last-Modified", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&chain[pos]);
        req->last_modified = value;
    }
    pos = chain.find("Location", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&chain[pos]);
        req->location = value;
    }
}

void       take_info_ter(std::string chain, t_request *req)
{
    int pos = -1;
    std::string value;

    pos = chain.find("Content-Language", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&chain[pos]);
        req->content_language = value;
    }
    pos = chain.find("Date", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&chain[pos]);
        req->date = value;
    }
    pos = chain.find("User-Agent", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&chain[pos]);
        req->user_agent = value;
    }
    pos = chain.find("Authorization", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&chain[pos]);
        req->authorization = value;
    }
    pos = chain.find("Allow", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&chain[pos]);
        req->allow = value;
    }
}

void       take_info_bis(std::string chain, t_request *req)
{
    int pos = -1;
    std::string value;

    pos = chain.find("Accept-Language", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&chain[pos]);
        req->accepted_language = value;
    }
    pos = chain.find("Accept-Charset", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&chain[pos]);
        req->accepted_charset = value;
    }
    pos = chain.find("Content-Length", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&chain[pos]);
        req->content_lenght = value;
    }
    pos = chain.find("Content-Location", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&chain[pos]);
        req->content_location = value;
    }
}

void    take_body(t_request *req, char *buffer)
{
    int i;

    i = strlen(buffer);
    while(i > 1 && buffer[i] != '\n' && buffer[i - 1] != '\n')
        i--;
    if (i > 1)
    {
        std::string str(buffer);
        req->content = str.substr(i, strlen(buffer) - i);
    }
}

t_request  takeInfo(char *buffer)
{
    t_request req;
    std::stringstream ss(buffer);
    std::string chain, value;
    size_t pos = -1;

    std::getline(ss, chain, ' ');
    req.request_method = chain;
    std::getline(ss, chain, ' ');
    req.path_info = "." + chain;
	std::getline(ss, chain, '\n');
    req.protocol = chain;
    chain = buffer;
    take_info_bis(chain, &req);
    take_info_ter(chain, &req);
    take_info_quater(chain, &req);
    take_info_cinquies(chain, &req);
    pos = chain.find("User-Agent", 0);
    if (pos != std::string::npos)
    {
        value = onlyTheGood(&chain[pos]);
        req.accepted_language = value;
    }
    pos = chain.find("WWW-Authenticate", 0);
    if (pos != std::string::npos)
    {
        value = onlyTheGood(&chain[pos]);
        req.accepted_language = value;
    }
    if (req.path_info.empty() == false && req.host.empty() == false)
        req.path_translated = req.host + req.path_info;
    take_body(&req, buffer);
    return (req);
}
