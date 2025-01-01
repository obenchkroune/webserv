/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 21:26:24 by simo              #+#    #+#             */
/*   Updated: 2025/01/01 22:30:17 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerClient.hpp"

void ServerClient::ProcessCGI(Response* response)
{
    throw NotImplemented();
    int pid = fork();

    if (pid)
    {
        /*
            Should forward cgi output pipe to IOmltplxr..
        */
    }
    else
    {
        std::vector<char*> argv;
        argv.insert(argv.end(), (char*)response->GetFileLocation()->cgi_path.c_str());
        argv.insert(argv.end(), (char*)response->GetFileName().c_str());
        execve(response->GetFileLocation()->cgi_path.c_str(), &(argv.front()), NULL);
    }
}
