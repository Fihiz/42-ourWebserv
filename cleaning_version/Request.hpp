/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgoudet <pgoudet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 08:08:07 by pgoudet           #+#    #+#             */
/*   Updated: 2021/06/10 13:10:43 by pgoudet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>

typedef struct s_request
{
    std::string accepted_charset; //+
    std::string accepted_language; //+
    std::string allow; //+
    std::string authorization; //+
    std::string content_language; //+
    std::string content_lenght; //+
    std::string content_location; //+
    std::string content_type; //+
    std::string date; //+
    std::string host; // +
    std::string last_modified; //+
    std::string location; //+
    std::string referer; //+
    std::string retry_after; //+
    std::string server; // +
    std::string transfert_encoding; //+
    std::string user_agent; //+
    std::string www_authenticate; //+

    std::string content; //+
    std::string protocol; // +
    std::string path_info; //+
    std::string request_method; // +
    std::string path_translated; // +
    std::string script;   // -
    std::string query_string; // -
    std::string uri; // -
    std::string remote_user; //-
    std::string remote_addr; // -
    std::string auth_type; //-

}               t_request;

#endif