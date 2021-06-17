/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsingRequest.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 16:58:01 by pgoudet           #+#    #+#             */
/*   Updated: 2021/06/17 18:38:07 by sad-aude         ###   ########lyon.fr   */
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
	while(tmp[i] && tmp[i] != ':')// && tmp[i + 1] != ' ')
		i++;
    if (tmp[i + 1] == ' ')
        i++;
	str = str.substr(i + 1, ss_str.length() - i - 1);
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

void    takeBody(t_request *req, char *requestBuffer)
{
    int i;

    i = strlen(requestBuffer);
    while(i > 1 && requestBuffer[i] != '\n' && requestBuffer[i - 1] != '\n')
        i--;
    if (i > 1)
    {
        std::string str(requestBuffer);
        req->body = str.substr(i, strlen(requestBuffer) - i);
    }
}

void        splittingPath(t_request *req)
{
    std::string fileName;
    std::string fileExt;
    std::string withoutFirstPoint = (req->pathInfo).substr(1);
    //std::cout << "path info: [" << withoutFirstPoint << "]" << std::endl;
    std::string::size_type dot = (withoutFirstPoint).find('.');
    if (dot != std::string::npos)
    {
        fileName = (req->pathInfo).substr(0, dot + 1);
        std::cout << "filename " << fileName <<std::endl;
        fileExt = (req->pathInfo).substr(dot + 1 + 1);
        std::cout << "fileExt " << fileExt <<std::endl;
    }
    else
    {
        fileName = (req->pathInfo);
        fileExt = "";
    }
    if (fileExt == "jpg") // to be : contentType = ft_ext_to_type(ext);
        req->fileType = "image/jpeg";
    else if (fileExt == "ico")
        req->fileType = "image/x-icon";
    else
        req->fileType = "text/html";
}

t_request  parsingRequest(char *requestBuffer)
{
    t_request req;
    std::stringstream ss(requestBuffer);
    std::string word, value;
    size_t pos = -1;

    std::getline(ss, word, ' ');
    req.requestMethod = word;
    std::getline(ss, word, ' ');
    req.pathInfo = "." + word;
	std::getline(ss, word, '\n');
    req.protocol = word;
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
    // Added by socket team
    splittingPath(&req);
    return (req);
}
