/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 21:26:24 by simo              #+#    #+#             */
/*   Updated: 2025/01/02 21:21:36 by simo             ###   ########.fr       */
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
        for (int fd = 3; fd <= 10; fd++)
        {
            struct stat st;
            if (fstat(fd, &st) == -1)
                std::cerr << "fd: " << fd << " is closed." << std::endl;
            else
                std::cerr << "fd: " << fd << " is open." << std::endl;
        }
        close(IOMultiplexer::GetInstance().GetEpollFd());
        std::map<int, ServerClient>::const_iterator clients = _server->GetClients().begin();
        for (; clients != _server->GetClients().end(); clients++)
            close(clients->first);
        std::vector<int>::const_iterator sockets = _server->GetListenSockets().begin();
        for (; sockets != _server->GetListenSockets().end(); sockets++)
            close(*sockets);
        
        
        close(pipe_fd[0]);
        if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
        {
            std::cerr << "dup2() failed for cgi file: " << response->GetFileName() << std::endl;
            IOMultiplexer::GetInstance().Terminate();
            exit(10);
        }
        close(pipe_fd[1]);
        
        std::cerr << "================================" << std::endl;
        for (int fd = 3; fd <= 10; fd++)
        {
            struct stat st;
            if (fstat(fd, &st) == -1)
                std::cerr << "fd: " << fd << " is closed." << std::endl;
            else
                std::cerr << "fd: " << fd << " is open." << std::endl;
        }
        std::vector<char*> argv(3, NULL);
        argv[0] = strdup(response->GetFileLocation()->cgi_path.c_str());
        argv[1] = strdup(response->GetFileName().c_str());
        execve(response->GetFileLocation()->cgi_path.c_str(), argv.data(), environ);
        std::cerr << "execve() failed for cgi file: " << response->GetFileName() << std::endl;
        exit(10);
    }
}
