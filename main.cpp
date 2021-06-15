/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgoudet <pgoudet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 09:22:44 by pgoudet           #+#    #+#             */
/*   Updated: 2021/06/15 14:23:41 by pgoudet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.hpp"
#include "all.hpp"

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
    req = take_info(tmp);
    test = check_header(req);
    if (test != 0)
    {
        write(1, "wrong file path\n", strlen("wrong file path\n"));
        return (-1);
    }
    // std::cout \
    // << "req.User-Agent=" << req.user_agent \
    // << "\naccepted_charset=" << req.accepted_charset \
    // << "\naccepted_language=" << req.accepted_language \
    // << "\nallow=" << req.allow \
    // << "\nauthorization=" << req.authorization \
    // << "\ncontent_language=" << req.content_language \
    // << "\ncontent_lenght=" << req.content_lenght \
    // << "\ncontent_location=" <<req.content_location \
    // << "\ncontent_type=" << req.content_type \
    // << "\ndate=" << req.date \
    // << "\nhost=" << req.host \
    // << "\nlast_modified=" << req.last_modified \
    // << "\nlocation=" << req.location \
    // << "\nreferer=" << req.referer \
    // << "\nretry_after=" << req.retry_after \
    // << "\nserver=" << req.server \
    // << "\ntransfert_encoding=" << req.transfert_encoding \
    // << "\nuser_agent=" << req.user_agent \
    // << "\nwww_authenticate=" << req.www_authenticate \
    // << "\nprotocol=" << req.protocol \
    // << "\npath_info=" << req.path_info \
    // << "\nrequest_method=" << req.request_method \
    // << std::endl;
    // std::cout << req.content << std::endl;
    if ((envp = init_envp(envp, req, serv)) == NULL)
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