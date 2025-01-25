/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 21:26:24 by simo              #+#    #+#             */
/*   Updated: 2025/01/25 22:11:17 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerClient.hpp"
#include <fcntl.h>
void ServerClient::ProcessCGI(Response* response)
{
    int pipe_fd[2];
    int cgi_input_fd = -1;
    if (pipe(pipe_fd) == -1)
    {
        std::cerr << "pipe() failed for cgi file [output]: " << response->GetFilePath()
                  << std::endl;
        return ServerUtils::SendErrorResponse(HttpStatus(STATUS_INTERNAL_SERVER_ERROR), response);
    }
    if (!response->GetRequest().isChunked() && response->GetRequest().getBody().size())
    {
        std::stringstream fname;
        fname << "/tmp/cgi_input_" << time(0);
        cgi_input_fd =
            open(fname.str().c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP);
        if (cgi_input_fd == -1)
        {
            close(pipe_fd[0]), close(pipe_fd[1]);
            return ServerUtils::SendErrorResponse(
                HttpStatus(STATUS_INTERNAL_SERVER_ERROR), response
            );
        }
        ssize_t bytes = write(
            cgi_input_fd, response->GetRequest().getBody().data(),
            response->GetRequest().getBody().size()
        );
        if (bytes != (ssize_t)response->GetRequest().getBody().size() ||
            lseek(cgi_input_fd, 0, SEEK_SET) == -1)
        {
            close(pipe_fd[0]), close(pipe_fd[1]), close(cgi_input_fd);
            return ServerUtils::SendErrorResponse(
                HttpStatus(STATUS_INTERNAL_SERVER_ERROR), response
            );
        }
    }
    if (response->GetRequest().isChunked())
    {
        cgi_input_fd = response->GetRequest().getBodyFd();
        if (lseek(cgi_input_fd, 0, SEEK_SET) == -1)
        {
            close(pipe_fd[0]), close(pipe_fd[1]), close(cgi_input_fd);
            return ServerUtils::SendErrorResponse(
                HttpStatus(STATUS_INTERNAL_SERVER_ERROR), response
            );
        }
    }
    int pid = fork();
    if (pid == -1)
    {
        std::cerr << "fork() failed for cgi file: " << response->GetFilePath() << std::endl;
        close(pipe_fd[0]), close(pipe_fd[1]), close(cgi_input_fd);
        return ServerUtils::SendErrorResponse(HttpStatus(STATUS_INTERNAL_SERVER_ERROR), response);
    }
    if (pid)
    {
        /*TODO: set response timeout*/
        close(pipe_fd[1]), close(cgi_input_fd);
        _server->QueueCGIResponse(pipe_fd[0], response);
    }
    else
    {
        /*duping STDOUT*/
        close(pipe_fd[0]);
        if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
        {
            std::cerr << "dup2() failed for cgi file: " << response->GetFilePath() << std::endl;
            exit(13);
        }
        close(pipe_fd[1]);
        if (cgi_input_fd != -1 && dup2(cgi_input_fd, STDIN_FILENO) == -1)
        {
            std::cerr << "dup2() failed for cgi file: " << response->GetFilePath() << std::endl;
            exit(13);
        }
        close(cgi_input_fd);
        /*setting up argv*/
        std::vector<char*> argv(3, NULL);
        argv[0] = (char*)response->GetFileLocation()->cgi_path.c_str();
        argv[1] = (char*)response->GetFilePath().c_str();
        /*setting up environment*/
        std::vector<char*> envp;
        std::string        env_redirect = "REDIRECT_STATUS=CGI";
        std::string        env_method   = "REQUEST_METHOD=" + response->GetRequest().getMethod();
        std::string        env_uri      = "REQUEST_URI=" + response->GetRequest().getUri();
        std::string env_query = "QUERY_STRING=" + response->GetRequest().getQueryParamsString();
        std::string env_script_name = "SCRIPT_FILENAME=";
        env_script_name += response->GetFilePath().c_str();
        const HttpHeader* cookies = response->GetRequest().getHeader("Cookie");
        std::string       env_cookies;
        if (cookies != NULL)
        {
            env_cookies = "HTTP_COOKIE=" + cookies->raw_value;
            envp.insert(envp.end(), (char*)env_cookies.c_str());
        }
        const HttpHeader* content_type = response->GetRequest().getContentTypeHeader();
        std::string       env_content_type;
        if (content_type != NULL)
        {
            env_content_type = "CONTENT_TYPE=" + content_type->raw_value;
            envp.insert(envp.end(), (char*)env_content_type.c_str());
        }
        std::stringstream content_lenght;
        if (response->GetRequest().isChunked())
            content_lenght << "CONTENT_LENGTH=" << response->GetRequest().getBodySize();
        else
            content_lenght << "CONTENT_LENGTH=" << response->GetRequest().getBody().size();
        std::string env_content_lenght = content_lenght.str();
        /*inserting environment*/
        envp.insert(envp.end(), (char*)env_redirect.c_str());
        envp.insert(envp.end(), (char*)env_method.c_str());
        envp.insert(envp.end(), (char*)env_uri.c_str());
        envp.insert(envp.end(), (char*)env_query.c_str());
        envp.insert(envp.end(), (char*)env_script_name.c_str());
        envp.insert(envp.end(), (char*)env_content_lenght.c_str());
        envp.insert(envp.end(), NULL);

        execve(response->GetFileLocation()->cgi_path.c_str(), argv.data(), envp.data());
        std::cerr << "execve() failed for cgi file: " << response->GetFilePath() << std::endl;
        exit(10);
    }
}
