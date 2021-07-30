/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CreatingPage.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/10 16:07:14 by sad-aude          #+#    #+#             */
/*   Updated: 2021/07/30 16:00:29 by sad-aude         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

std::string     getFileContent(std::string fullFileName)
{
    std::ifstream		ifs(fullFileName);
    std::string			content;
    std::stringstream	line;
    
	if (!ifs.good())
        return (content);

	try
    {
        line << ifs.rdbuf();
        content = line.str();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
	}

    ifs.close();
    return (content);
}

std::string     getDefaultErrorPage(std::string causeError) {
	std::string content;
	content += "<!DOCTYPE html>";
	content += "<html>";
	content += "<head><title>" + causeError + "</title></head>";
	content += "<body>";
	content += "<center><h1>" + causeError + "</h1></center>";
	content += "<hr><center>webserv/101.42</center>";
	content += "</body>";
	content += "</html>";
	return (content);
}

std::string     getContentFileError(Config * virtualHost, std::string causeError) {
    std::string contentFile;
    struct stat statBuf;
    if (virtualHost != NULL) {
        std::map<int, std::string> listError;
        std::map<int, std::string>::const_iterator findError;
        listError = virtualHost->getErrorPage();
        findError = listError.find(atoi(causeError.c_str()));
        if (findError != listError.end() && (stat(findError->second.c_str(), &statBuf) != -1))
            contentFile = getFileContent(findError->second);
        else
            contentFile = getDefaultErrorPage(causeError);
    }
    else
        contentFile = getDefaultErrorPage("400 Bad Request");
    return (contentFile);
}

void            setContentDependingOnFileOrDirectory(t_request &parsedRequest, const t_location *loc, Config * conf)
{
    struct stat statBuf;
    int ret = stat((parsedRequest.fullPathInfo).c_str(), &statBuf); /* need to be protected  */
    if (ret == -1)
    {
        parsedRequest.statusCode = "404 Not Found";
        parsedRequest.fileContent = getContentFileError(conf, parsedRequest.statusCode);
    }
    else
    {   
        if (!S_ISDIR(statBuf.st_mode))
            parsedRequest.fileContent = getFileContent(parsedRequest.fullPathInfo);
        else
        {
            std::string tmpFileName;
            std::string tmpFullPathInfo = parsedRequest.fullPathInfo;
            for (std::vector<std::string>::const_iterator it = loc->index.begin(); it != loc->index.end(); it++)
            {
                tmpFileName = loc->root + *it;
                // ret = stat(tmpFileName.c_str(), &statBuf);
                parsedRequest.fullPathInfo = tmpFileName;
                ret = checkPath(parsedRequest);
                if (ret != 0)
                    break;
            }
            parsedRequest.fullPathInfo = tmpFullPathInfo;
            if (ret == 0)
            {
                if (loc->autoindex)
                {
                    std::cout << T_GYB "AUTO INDEX HAS BEEN ASKED " T_N;
                    std::cout << T_GYB "parsedRequest.pathInfo: " T_N << parsedRequest.pathInfo << std::endl;
                    parsedRequest.fileContent = createAutoIndex(parsedRequest.fullPathInfo, parsedRequest.pathInfo);
                }
                else
                {
                    parsedRequest.statusCode = "403 Forbidden";
                    parsedRequest.fileContent = getContentFileError(conf, parsedRequest.statusCode);
                }
            }
            else
                parsedRequest.fileContent = getFileContent(tmpFileName);
        }
    }
}
