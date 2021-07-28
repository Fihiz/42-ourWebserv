/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndex.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/10 16:07:14 by sad-aude          #+#    #+#             */
/*   Updated: 2021/07/28 17:05:09 by sad-aude         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

std::string numFormat(int nb)
{
    char    itoaTab[3];

    if (sprintf(itoaTab, "%d", nb) < 0)
        return ("");
	else if (nb < 10)
		return ("0" + (std::string)itoaTab);
	return (itoaTab);
}

std::string getDateFormat( time_t &date )
{
	std::string	ret;
	std::string month[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	struct tm	*formated;
    char    itoaTab[10];

	formated = gmtime(&date);
    if (formated && (sprintf(itoaTab, "%d", 1900 + formated->tm_year) > 0))
    {
        ret = numFormat(formated->tm_mday) + "-" + month[formated->tm_mon] + "-" + (std::string)itoaTab;
        ret += " " + numFormat(formated->tm_hour) + ":" + numFormat(formated->tm_min);
    }
	return (ret);
}

std::string formatName( std::string src )
{
    std::string spaces = " ";

    if (src.size() == 50)
        ;
    else if ((src.size() == 51) && (src[50] == '/'))
        src[50] = '\0';
    else if (src.size() > 50)
        src = src.substr(0, 47) + "..>";
    src = src + "</a>";
    while (src.size() + spaces.size() < 55)
        spaces = spaces + " ";
    return(src + spaces);
}

std::string     fillAutoIndex( std::vector<std::string> &files, std::string &fileName, std::string &fullFileName )
{
    std::string content = "<html>\n\t<head><title>Index of " + fileName + "</title></head>\n\t<body bgcolor=\"white\">\n\t<h1>Index of " + fileName + "</h1><hr><pre>\n";
    std::vector<std::string>::iterator   it = files.begin();
    std::string dir_buf = "<a href=\"../\">../</a>\n"; 
    std::string files_buf = "";
    struct stat buf;
    char    itoaTab[100];

    while (it != files.end())
    {
        if ((*it)[0] != '.')
        {
            stat((fullFileName + *it).c_str(), &buf);
            if (S_ISDIR(buf.st_mode))
                dir_buf += "<a href=\"" + *it + "/\">" + formatName(*it + "/") + getDateFormat(buf.st_mtime) + "                   " + "-" + "\n";
            else if (sprintf(itoaTab, "%lld", buf.st_size) > 0)
                files_buf += "<a href=\"" + *it + "\">" + formatName(*it) + getDateFormat(buf.st_mtime) + "                   " + (std::string)itoaTab + "\n";
            else
                files_buf += "<a href=\"" + *it + "\">" + formatName(*it) + getDateFormat(buf.st_mtime) + "                   nan" + "\n";
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

    if ((dir = opendir((fullFileName).c_str())) != nullptr)
    {
        while ((dirRead = readdir(dir)) != nullptr)
        {
            files.push_back((std::string)dirRead->d_name);
        }
        closedir (dir);
    }
    else
    {
        perror ("opendir");
        return (NULL);
    }
    return (fillAutoIndex(files, fileName, fullFileName));
}
