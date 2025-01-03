/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 21:26:24 by simo              #+#    #+#             */
/*   Updated: 2025/01/02 23:05:46 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerClient.hpp"
void ServerClient::ProcessCGI(Response* response)
{
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
        _server->QueueCGIResponse(pipe_fd[0], response);
    }
    else
    {
        close(pipe_fd[0]);
        if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
        {
            std::cerr << "dup2() failed for cgi file: " << response->GetFileName() << std::endl;
            exit(13);
        }
        close(pipe_fd[1]);
        pipe(pipe_fd);
        std::string raw_request_headers(response->GetRequest().getRawBuffer().str());
        write(pipe_fd[1], raw_request_headers.c_str(), raw_request_headers.size());
        close(pipe_fd[1]);
        if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
        {
            std::cerr << "dup2() failed for cgi file: " << response->GetFileName() << std::endl;
            exit(12);
        }
        std::vector<char*> argv(3, NULL);
        argv[0] = strdup(response->GetFileLocation()->cgi_path.c_str());
        argv[1] = strdup(response->GetFileName().c_str());
        execve(response->GetFileLocation()->cgi_path.c_str(), argv.data(), environ);
        std::cerr << "execve() failed for cgi file: " << response->GetFileName() << std::endl;
        exit(10);
    }
}
