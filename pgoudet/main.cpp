/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 09:22:44 by pgoudet           #+#    #+#             */
/*   Updated: 2021/06/17 18:39:34 by sad-aude         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "request.hpp"

int main(int ac, char *av[], char *envp[])
{
    std::string buffer;
    t_request req;
    char *tmp;
    t_serv     serv;
    pid_t pid;
    int test;

    serv.server_name = reinterpret_cast<const char *>("localhost");
    serv.server_port = reinterpret_cast<const char *>("1");
    serv.server_protocol = reinterpret_cast<const char *>("HTTP/1.1");
    buffer = "GET test2.php HTTP/1.1\nHost: developer.mozilla.org\nAccept-Language: fr";
    //buffer = "POST test2.php HTTP/1.1\nHost: foo.com\nContent-Type: application/x-www-form-urlencoded\n\n\nContent-Length: 13\n\nsay=Hi&to=Mom";
    tmp = const_cast<char *>(buffer.c_str());
    req = parsingRequest(tmp);
    test = checkingHeader(req);
    if (test != 0)
    {
        write(1, "wrong file path\n", strlen("wrong file path\n"));
        return (-1);
    }
    // std::cout \
    // << "req.User-Agent=" << req.userAgent \
    // << "\nacceptedCharset=" << req.acceptedCharset \
    // << "\nacceptedLanguage=" << req.acceptedLanguage \
    // << "\nallow=" << req.allow \
    // << "\nauthorization=" << req.authorization \
    // << "\ncontentLanguage=" << req.contentLanguage \
    // << "\ncontentLenght=" << req.contentLenght \
    // << "\ncontentLocation=" <<req.contentLocation \
    // << "\ncontentType=" << req.contentType \
    // << "\ndate=" << req.date \
    // << "\nhost=" << req.host \
    // << "\nlastModified=" << req.lastModified \
    // << "\nlocation=" << req.location \
    // << "\nreferer=" << req.referer \
    // << "\nretryAfter=" << req.retryAfter \
    // << "\nserver=" << req.server \
    // << "\ntransfertEncoding=" << req.transfertEncoding \
    // << "\nuserAgent=" << req.userAgent \
    // << "\nwwwAuthenticate=" << req.wwwAuthenticate \
    // << "\nprotocol=" << req.protocol \
    // << "\npathInfo=" << req.pathInfo \
    // << "\nrequestMethod=" << req.requestMethod \
    // << std::endl;
    // std::cout << req.content << std::endl;
    if ((envp = initEnv(envp, req, serv)) == NULL)
    {
        write(1, "bad request\n", strlen("bad request\n"));
        return (1);
    }
    // for (int i = 0; i < tab_size(envp); i++)
    //     std::cout << envp[i] << std::endl;
    pid = fork();
	if (pid == 0)
	{
		test = execve("./cgi", av, envp);
        std::cout << test << std::endl;
        std::cout << errno << std::endl;
        perror("");
		if (test == -1)
			exit(-1);
	}
	else
		waitpid(-1, NULL, 0);
	if (errno != 0)
	    return (errno);
    return (0);
}