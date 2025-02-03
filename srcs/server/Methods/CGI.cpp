/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 21:26:24 by simo              #+#    #+#             */
/*   Updated: 2025/02/03 13:54:50 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerClient.hpp"
#include <fcntl.h>
void ServerClient::ProcessCGI(ResponseCGI* response)
{
    int pipe_fd[2];
    int cgi_input_fd = response->GetRequest().getBodyFd();
    if (pipe(pipe_fd) == -1)
    {
        std::cerr << "pipe() failed for the [output] of cgi script file: "
                  << response->GetRequestFilePath() << std::endl;
        return ServerClient::SendErrorResponse(HttpStatus(STATUS_INTERNAL_SERVER_ERROR), response);
    }
    int pid = fork();
    if (pid == -1)
    {
        std::cerr << "fork() failed for cgi script file: " << response->GetRequestFilePath()
                  << std::endl;
        close(pipe_fd[0]), close(pipe_fd[1]);
        return ServerClient::SendErrorResponse(HttpStatus(STATUS_INTERNAL_SERVER_ERROR), response);
    }
    if (pid)
    {
        response->SetCGIPID(pid);
        close(pipe_fd[1]), close(cgi_input_fd);
        QueueCGIResponse(pipe_fd[0], response);
    }
    else
    {
        /*duping STDOUT*/
        close(pipe_fd[0]);
        if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
        {
            std::cerr << "dup2() failed for the [output] of cgi script file: "
                      << response->GetRequestFilePath() << std::endl;
            exit(13);
        }
        close(pipe_fd[1]);
        if (cgi_input_fd != -1 && dup2(cgi_input_fd, STDIN_FILENO) == -1)
        {
            std::cerr << "dup2() failed for the [input] of cgi script file: "
                      << response->GetRequestFilePath() << std::endl;
            exit(13);
        }
        close(cgi_input_fd);
        /*setting up argv*/
        std::vector<char*> argv(3, NULL);
        argv[0] = (char*)response->GetRequest().getRequestFileLocation()->cgi_path.c_str();
        argv[1] = (char*)response->GetRequestFilePath().c_str();
        if (response->GetRequest().getRequestFileLocation()->cgi_conf != "")
        {
            argv[2] = (char*)"-c";
            argv.insert(
                argv.end(), (char*)response->GetRequest().getRequestFileLocation()->cgi_conf.c_str()
            );
            argv.insert(argv.end(), NULL);
        }
        /*setting up environment*/
        std::vector<char*> envp;
        std::string        env_redirect = "REDIRECT_STATUS=CGI";
        std::string        env_method   = "REQUEST_METHOD=" + response->GetRequest().getMethod();
        std::string        env_uri      = "REQUEST_URI=" + response->GetRequest().getUri();
        std::string env_query = "QUERY_STRING=" + response->GetRequest().getQueryParamsString();
        std::string env_script_name = "SCRIPT_FILENAME=";
        env_script_name += response->GetRequestFilePath().c_str();
        const HttpHeader* cookies = response->GetRequest().getHeader("Cookie");
        std::string       env_cookies;
        if (cookies != NULL)
        {
            env_cookies = "HTTP_COOKIE=" + cookies->raw_value;
            envp.insert(envp.end(), (char*)env_cookies.c_str());
        }
        const HttpHeader* content_type = response->GetRequest().getHeader("Content-Type");
        std::string       env_content_type;
        if (content_type != NULL)
        {
            env_content_type = "CONTENT_TYPE=" + content_type->raw_value;
            envp.insert(envp.end(), (char*)env_content_type.c_str());
        }
        std::stringstream content_lenght;
        content_lenght << "CONTENT_LENGTH=" << response->GetRequest().getBodySize();
        std::string env_content_lenght = content_lenght.str();
        /*inserting environment*/
        envp.insert(envp.end(), (char*)env_redirect.c_str());
        envp.insert(envp.end(), (char*)env_method.c_str());
        envp.insert(envp.end(), (char*)env_uri.c_str());
        envp.insert(envp.end(), (char*)env_query.c_str());
        envp.insert(envp.end(), (char*)env_script_name.c_str());
        envp.insert(envp.end(), (char*)env_content_lenght.c_str());
        envp.insert(envp.end(), NULL);

        std::cerr << ">>>> [CGI variables] " << std::endl;
        for (char** env = envp.data(); env && *env; env++)
            std::cerr << *env << std::endl;
        std::cerr << ">>>> [END CGI variables] " << std::endl;
        execve(
            response->GetRequest().getRequestFileLocation()->cgi_path.c_str(), argv.data(),
            envp.data()
        );
        std::cerr << "execve() failed for cgi script file: " << response->GetRequestFilePath()
                  << std::endl;
        exit(10);
    }
}
