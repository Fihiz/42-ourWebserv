/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PageCreation.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/10 16:07:14 by sad-aude          #+#    #+#             */
/*   Updated: 2021/06/15 14:05:14 by sad-aude         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

void    getFileInfo(char *request, std::string &fileName, std::string &ext, std::string &contentType)
{
    std::string::size_type n = ((std::string)(request)).find('/');
    if (n != std::string::npos && (n < ((std::string)(request)).find('\n')))
    {
        std::string::size_type dot = ((std::string)(request + n + 1)).find('.');
        if ((dot != std::string::npos) && (dot < ((std::string)(request + n + 1)).find(' '))) // si on trouve un ' ' avant de trouver un '.', on nous demande pas un fichier
        {
            std::string::size_type space = ((std::string)(request + n + 1 + dot + 1)).find(' '); // space pos relative to dot pos
            fileName = ((std::string)(request)).substr(n + 1, dot);
            ext = ((std::string)(request)).substr(n + 1 + dot + 1, space);
        }
        else
        {
            fileName = "./" + ((std::string)(request)).substr(n + 1, ((std::string)(request + n + 1)).find(' '));
            ext = "";
        }
    }
    else // si la requête a pas de nom de fichier sur la première ligne
    {
        fileName = "html/index";
        ext = "html";
    }

    if (ext == "jpg") // to be : contentType = ft_ext_to_type(ext);
        contentType = "image/jpeg";
    else if (ext == "ico")
        contentType = "image/x-icon";
    else
        contentType = "text/html";
}

std::string getFileContent(std::string fullFileName)
{
    std::ifstream	ifs(fullFileName); // Dealing with protection later
    std::stringstream   line;
    line << ifs.rdbuf();
    std::string content = line.str();
    ifs.close();

    return (content);
}

std::string numFormat(int nb)
{
	if (nb < 10)
		return ("0" + std::to_string(nb));
	return (std::to_string(nb));
}

std::string	getDateFormat(time_t	&date)
{
	std::string	ret;
	std::string month[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	struct tm	*formated;

	formated = gmtime(&date);
	ret = numFormat(formated->tm_mday) + "-" + month[formated->tm_mon] + "-" + std::to_string(1900 + formated->tm_year);
	ret += " " + numFormat(formated->tm_hour) + ":" + numFormat(formated->tm_min);
	return (ret);
}

/* AUTO INDEX */

std::string     fillAutoIndex( std::vector<char *> &files, std::string &fileName )
{
    std::string content = "<html>\n\t<head><title>Index of /</title></head>\n\t<body bgcolor=\"white\">\n\t<h1>Index of /</h1><hr><pre>\n";
    std::vector<char *>::iterator   it = files.begin();
    std::string dir_buf = ""; // To remove later 
    std::string files_buf = "";
    struct stat buf;

    std::cout << T_CB "JE PASSE ICI |" << fileName << "|" << *(files.begin()) << " | " << *(files.end() - 1) << " | " T_N << std::endl;
    while (it != files.end())
    {
        if ((*it)[0] != '.' || (((std::string)(*it)) == ".."))
        {
            std::cerr << "file:" << *it << std::endl;
            if (fileName[fileName.size()] == '/')
                stat((fileName + (std::string) *it).c_str(), &buf);
            else
                stat((fileName + "/" + (std::string) *it).c_str(), &buf);
            std::cout << T_GYB << *it << " " << S_ISDIR(buf.st_mode) << " " << S_ISREG(buf.st_mode) << T_N << std::endl;
            if (S_ISDIR(buf.st_mode))
                dir_buf += "<a href=\"" + (std::string)*it + "/\">" + (std::string)*it + "/</a>" + "                                   " + getDateFormat(buf.st_mtime) + "                   " + "-" + "\n";
            else
                files_buf += "<a href=\"" + (std::string)*it + "\">" + (std::string)*it + "</a>" + "                                   " + getDateFormat(buf.st_mtime) + "                   " + std::to_string(buf.st_size) + "\n";
        }
        it++;
    }
    return (content += dir_buf + files_buf + "\t</pre><hr></body>\n</html>");
}

std::string     createAutoIndex( std::string &fileName )
{
    DIR *dir;
    std::vector<char *> files;
    struct dirent *dirRead;

    std::cout << T_CB << fileName << " begin: " << (files.begin() == files.end()) << T_N << std::endl;
    if ((dir = opendir((fileName).c_str())) != nullptr)
    {
        while ((dirRead = readdir(dir)) != nullptr)
        {  
            std::cout << T_RB "dir name : " << dirRead->d_name << T_N << std::endl;
            files.push_back(dirRead->d_name);
        }
        closedir (dir);
    }
    else
    {
        perror ("opendir");
        return (NULL);
    }
    return (fillAutoIndex(files, fileName));
    //std::cout << content << std::endl;
}

