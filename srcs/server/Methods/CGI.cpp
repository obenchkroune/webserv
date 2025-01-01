/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 21:26:24 by simo              #+#    #+#             */
/*   Updated: 2025/01/01 23:55:53 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerClient.hpp"

void ServerClient::ProcessCGI(Response* response)
{
    throw NotImplemented();
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1)
    {
        std::cerr << "pipe() failed for cgi file: " << response->GetFileName() << std::endl;
        return SendErrorResponse(
            HttpStatus(STATUS_INTERNAL_SERVER_ERROR, HTTP_STATUS_INTERNAL_SERVER_ERROR), response
        );
    }
    int pid = fork();
    if (pid == -1)
    {
        std::cerr << "fork() failed for cgi file: " << response->GetFileName() << std::endl;
        close(pipe_fd[0]), close(pipe_fd[1]);
        return SendErrorResponse(
            HttpStatus(STATUS_INTERNAL_SERVER_ERROR, HTTP_STATUS_INTERNAL_SERVER_ERROR), response
        );
    }
    if (pid)
    {
        close(pipe_fd[1]);
        /*
            Should forward cgi output pipe to IOmltplxr..
        */
    }
    else
    {
        close(pipe_fd[0]);
        if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
        {
            std::cerr << "dup2() failed for cgi file: " << response->GetFileName() << std::endl;
            IOMultiplexer::GetInstance().Terminate();
            exit(10);
        }
        close(pipe_fd[1]);
        std::vector<char*> argv;
        argv.insert(argv.end(), (char*)response->GetFileLocation()->cgi_path.c_str());
        argv.insert(argv.end(), (char*)response->GetFileName().c_str());
        execve(response->GetFileLocation()->cgi_path.c_str(), &(argv.front()), NULL);
        std::cerr << "fork() failed for cgi file: " << response->GetFileName() << std::endl;
        IOMultiplexer::GetInstance().Terminate();
        exit(10);
    }
}
