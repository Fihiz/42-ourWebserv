/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Answer.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 08:26:22 by pgoudet           #+#    #+#             */
/*   Updated: 2021/06/17 18:37:40 by sad-aude         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

// int getAnswer(t_request const &req, char **argv, char **envp)
// {
//     int pid, test, status;

//     pid = fork();
//     waitpid(-1, &status, 0);
// 	if (pid == 0)
// 	{
// 		test = execve("./cgi", argv, envp);
// 		if (test == -1)
// 			exit (-1);
// 	}
//     else
//         return (0);
// }

// int postAnswer(t_request const &req, char **argv, char **envp)
// {
//     int pid, test, status;

//     pid = fork();
//     waitpid(-1, &status, 0);
// 	if (pid == 0)
// 	{
// 		test = execve("./cgi", argv, envp);
// 		if (test == -1)
// 			exit (-1);
// 	}
//     else
//         return (0);
// }

// int deleteAnswer(t_request const &req)
// {
//     if((remove(req.pathInfo) != 0))
//         perror( "Error deleting file" );
//     else
//     {   std::cout << "HTTP/1.1 200 OK\nDate: Wed, 21 Oct 2015 07:28:00 GMT\n";
//         std::cout << "<html>\n";
//         std::cout << "  <body>\n";
//         std::cout << "    <h1>File deleted.</h1>\n";
//         std::cout << "  </body>\n";
//         std::cout << "</html>\n";
//     }
//     return 0;
// }