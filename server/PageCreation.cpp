/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PageCreation.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgoudet <pgoudet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/10 16:07:14 by sad-aude          #+#    #+#             */
/*   Updated: 2021/07/27 10:08:51 by pgoudet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

std::string getFileContent(std::string fullFileName)
{
    std::ifstream	ifs(fullFileName); // Dealing with protection later
    std::stringstream   line;
    line << ifs.rdbuf();
    std::string content = line.str();
    ifs.close();

    return (content);
}

std::string getDefaultErrorPage(std::string causeError) {
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

std::string getContentFileError(Config * virtualHost, std::string causeError) {
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

std::string numFormat(int nb)
{
	if (nb < 10)
		return ("0" + std::to_string(nb));
	return (std::to_string(nb));
}

std::string	getDateFormat( time_t &date )
{
	std::string	ret;
	std::string month[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	struct tm	*formated;

	formated = gmtime(&date);
	ret = numFormat(formated->tm_mday) + "-" + month[formated->tm_mon] + "-" + std::to_string(1900 + formated->tm_year);
	ret += " " + numFormat(formated->tm_hour) + ":" + numFormat(formated->tm_min);
	return (ret);
}

std::string formatName( std::string src )
{
    std::string spaces = " ";

    if (src.size() == 50)
        ;
    else if ((src.size() == 51) && (src[50] == '/'))
    {
        src[50] = '\0';
        //std::cout << "|" + src + "|" << std::endl;
    }
    else if (src.size() > 50)
        src = src.substr(0, 47) + "..>";
    src = src + "</a>";
    while (src.size() + spaces.size() < 55)
        spaces = spaces + " ";
    return(src + spaces);
}

void    setContentDependingOnFileOrDirectory(t_request &parsedRequest, const t_location *loc, Config * conf)
{
    struct stat statBuf;
    int ret = stat((parsedRequest.fullPathInfo).c_str(), &statBuf); /* need to be protected  */
    if (ret == -1)
    {
        parsedRequest.statusCode = "404 Not Found";
        parsedRequest.fileContent = getContentFileError(conf, parsedRequest.statusCode);
        // parsedRequest.fullPathInfo = "./pages/404.html";
    }
    else
    {
        if (!S_ISDIR(statBuf.st_mode))
            parsedRequest.fileContent = getFileContent(parsedRequest.fullPathInfo);
        else
        {
            std::string tmpFileName;
            ret = -1;
            for (std::vector<std::string>::const_iterator it = loc->index.cbegin(); it != loc->index.cend(); it++)
            {
                tmpFileName = parsedRequest.fullPathInfo + *it;
                ret = stat(tmpFileName.c_str(), &statBuf);
                if (ret != -1)
                    break;
            }
            if (ret == -1)
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
                    // parsedRequest.fullPathInfo = "./pages/403.html";
                    parsedRequest.fileContent = getContentFileError(conf, parsedRequest.statusCode);
                    // parsedRequest.fileContent = getFileContent(parsedRequest.fullPathInfo);
                }
            }
            else
            {
                parsedRequest.fileContent = getFileContent(tmpFileName);
            }
            //chercher le premier index de la location auquel on a acces
            //si on trouve on get le content du fichier
            //si y en a pas, if (autoindex == on)
            // std::cout << T_GYB "AUTO INDEX HAS BEEN ASKED " T_N;
            // std::cout << T_GYB "parsedRequest.pathInfo: " T_N << parsedRequest.pathInfo << std::endl;
            // parsedRequest.fileContent = createAutoIndex(parsedRequest.fullPathInfo, parsedRequest.pathInfo);
            //else 
            /* return 404 ?*/
        }
    }
}

/* AUTO INDEX */
std::string     fillAutoIndex( std::vector<std::string> &files, std::string &fileName )
{
    std::string content = "<html>\n\t<head><title>Index of /" + fileName.substr(2) + "</title></head>\n\t<body bgcolor=\"white\">\n\t<h1>Index of /" + fileName.substr(2) + "</h1><hr><pre>\n";
    std::vector<std::string>::iterator   it = files.begin();
    std::string dir_buf = "<a href=\"../\">../</a>\n"; 
    std::string files_buf = "";
    struct stat buf;

    while (it != files.end())
    {
        if ((*it)[0] != '.')
        {
            if (fileName[fileName.size() - 1] == '/')
                stat((fileName + *it).c_str(), &buf);
            else
                stat((fileName + "/" + *it).c_str(), &buf);
            //printf("%p\n", &(*it));
            // std::cout << T_GYB << *it << " " << S_ISDIR(buf.st_mode) << " " << S_ISREG(buf.st_mode) << T_N << std::endl;
            if (S_ISDIR(buf.st_mode))
                dir_buf += "<a href=\"" + *it + "/\">" + formatName(*it + "/") + getDateFormat(buf.st_mtime) + "                   " + "-" + "\n";
            else
                files_buf += "<a href=\"" + *it + "\">" + formatName(*it) +getDateFormat(buf.st_mtime) + "                   " + std::to_string(buf.st_size) + "\n";
        }
        it++;
    }
    return (content += dir_buf + files_buf + "\t</pre><hr></body>\n</html>");
}

std::string     createAutoIndex( std::string &fullFileName, std::string &fileName )
{
    DIR *dir;
    std::vector<std::string> files;
    struct dirent *dirRead;

    // std::cout << T_CB << fileName << " begin: " << (files.begin() == files.end()) << T_N << std::endl;
    if ((dir = opendir((fullFileName).c_str())) != nullptr)
    {
        while ((dirRead = readdir(dir)) != nullptr)
        {  
            // std::cout << T_RB "dir name : " << dirRead->d_name << T_N << std::endl;
            files.push_back((std::string)dirRead->d_name);
        }
        closedir (dir);
    }
    else
    {
        perror ("opendir");
        return (NULL);
    }
    return (fillAutoIndex(files, fileName));
}

