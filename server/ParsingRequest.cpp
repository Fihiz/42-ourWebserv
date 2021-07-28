/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsingRequest.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgoudet <pgoudet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 16:58:01 by pgoudet           #+#    #+#             */
/*   Updated: 2021/07/28 15:01:11 by pgoudet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

std::string onlyTheGood(std::string str)
{
    std::stringstream ss(str);
    std::string      ss_str;
	char *tmp = const_cast<char *>(str.c_str());
	int i = 0;
    
    std::getline(ss, ss_str);
	while(tmp[i] && tmp[i] != ':')
		i++;
    if (tmp[i + 1] == ' ')
        i++;
	str = str.substr(i + 1, ss_str.length() - i - 2);
	return (str);
}

void        parsingRequestCinquies(std::string word, t_request *req)
{
    int pos = -1;
    std::string value;

    pos = word.find("Referer", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&word[pos]);
        req->referer = value;
    }
    pos = word.find("Retry-After", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&word[pos]);
        req->retryAfter = value;
    }
    pos = word.find("Server", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&word[pos]);
        req->server = value;
    }
    pos = word.find("Transfert-Encoding", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&word[pos]);
        req->transfertEncoding = value;
    }
}

void       parsingRequestQuater(std::string word, t_request *req)
{
    int pos = -1;
    std::string value;

    pos = word.find("Content-Type", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&word[pos]);
        req->contentType = value;
    }
    pos = word.find("Host", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&word[pos]);
        req->host = value;
    }
    pos = word.find("Last-Modified", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&word[pos]);
        req->lastModified = value;
    }
    pos = word.find("Location", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&word[pos]);
        req->location = value;
    }
}

void       parsingRequestTer(std::string word, t_request *req)
{
    int pos = -1;
    std::string value;

    pos = word.find("Content-Language", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&word[pos]);
        req->contentLanguage = value;
    }
    pos = word.find("Date", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&word[pos]);
        req->date = value;
    }
    pos = word.find("User-Agent", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&word[pos]);
        req->userAgent = value;
    }
    pos = word.find("Authorization", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&word[pos]);
        req->authorization = value;
    }
    pos = word.find("Allow", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&word[pos]);
        req->allow = value;
    }
}

void       parsingRequestBis(std::string word, t_request *req)
{
    int pos = -1;
    std::string value;

    pos = word.find("Accept-Language", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&word[pos]);
        req->acceptedLanguage = value;
    }
    pos = word.find("Accept-Charset", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&word[pos]);
        req->acceptedCharset = value;
    }
    pos = word.find("Content-Length", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&word[pos]);
        req->contentLenght = value;
    }
    pos = word.find("Content-Location", 0);
    if (pos != (int)std::string::npos)
    {
        value = onlyTheGood(&word[pos]);
        req->contentLocation = value;
    }
}

void    takeBody(t_request *req, std::string requestBuffer)
{
    int i;

    i = requestBuffer.size();
    while(i > 1 && requestBuffer[i] != '\n' && requestBuffer[i - 1] != '\n')
        i--;
    if (i > 1)
    {
        std::string str(requestBuffer);
        req->body = str.substr(i, requestBuffer.size() - i);
    }
}

void        setFileTypeForResponse(t_request *req)
{
    std::string::size_type dot = (req->pathInfo).find('.');
    if (dot != std::string::npos)
        req->fileExt = (req->pathInfo).substr(dot);
    else
        req->fileExt = "";
    if (req->fileExt == "jpg")
        req->fileType = "image/jpeg";
    else if (req->fileExt == "ico")
    {
        std::cout << "\t🐄🐄🐄🐄🐄🐄🐄🐄..." << std::endl; 
        req->fileType = "image/x-icon";
    }
    else if (req->fileExt == "gif")
        req->fileType = "image/gif";
    else
        req->fileType = "text/html";
}

t_request  parsingRequest(std::string requestBuffer)
{
    t_request req;
    std::stringstream ss(requestBuffer);
    std::string word, value;
    size_t pos = -1;

    std::getline(ss, word, ' ');
    req.requestMethod = word;
    std::getline(ss, word, ' ');
    req.pathInfo = word;
	std::getline(ss, word, '\n');
    req.protocol = word.substr(0, word.size() - 1);
    checkingProtocol(req);
    if (req.statusCode == "400 Bad Request")
        return (req);
    word = requestBuffer;
    parsingRequestBis(word, &req);
    parsingRequestTer(word, &req);
    parsingRequestQuater(word, &req);
    parsingRequestCinquies(word, &req);
    pos = word.find("User-Agent", 0);
    if (pos != std::string::npos)
    {
        value = onlyTheGood(&word[pos]);
        req.acceptedLanguage = value;
    }
    pos = word.find("WWW-Authenticate", 0);
    if (pos != std::string::npos)
    {
        value = onlyTheGood(&word[pos]);
        req.acceptedLanguage = value;
    }
    if (req.pathInfo.empty() == false && req.host.empty() == false)
        req.pathTranslated = req.host + req.pathInfo;
    takeBody(&req, requestBuffer);
    setFileTypeForResponse(&req);
    return (req);
}
