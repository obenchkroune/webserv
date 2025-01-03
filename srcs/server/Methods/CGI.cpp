/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 21:26:24 by simo              #+#    #+#             */
/*   Updated: 2025/01/03 22:31:46 by simo             ###   ########.fr       */
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

        std::vector<char*> argv(3, NULL);
        argv[0] = (char*)response->GetFileLocation()->cgi_path.c_str();
        argv[1] = (char*)response->GetFileName().c_str();

        std::vector<char*> envp;
        std::string        env_redirect = "REDIRECT_STATUS=CGI";
        std::string        env_method =
            "REQUEST_METHOD=" + ServerUtils::HttpMethodToString(response->GetRequest().getMethod());
        std::string env_uri   = "REQUEST_URI=" + response->GetRequest().getUri();
        std::string env_query = "QUERY_STRING=" + response->GetRequest().getQueryParamsString();
        std::string env_script_name = "SCRIPT_FILENAME=";
        env_script_name += response->GetFileName().c_str();
        envp.insert(envp.end(), (char*)env_redirect.c_str());
        envp.insert(envp.end(), (char*)env_method.c_str());
        envp.insert(envp.end(), (char*)env_uri.c_str());
        envp.insert(envp.end(), (char*)env_query.c_str());
        envp.insert(envp.end(), (char*)env_script_name.c_str());
        // const char** environ = _server->GetEnviron();
        // for (; environ != NULL && *environ != NULL; environ++)
        //     envp.insert(envp.end(), (char*)*environ);
        envp.insert(envp.end(), NULL);

        execve(response->GetFileLocation()->cgi_path.c_str(), argv.data(), envp.data());
        std::cerr << "execve() failed for cgi file: " << response->GetFileName() << std::endl;
        exit(10);
    }
}
