/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 21:26:24 by simo              #+#    #+#             */
/*   Updated: 2025/01/09 15:17:31 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerClient.hpp"
void ServerClient::ProcessCGI(Response* response)
{
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1)
    {
        std::cerr << "pipe() failed for cgi file: " << response->GetFilePath() << std::endl;
        return ServerUtils::SendErrorResponse(
            HttpStatus(STATUS_INTERNAL_SERVER_ERROR, HTTP_STATUS_INTERNAL_SERVER_ERROR), response
        );
    }
    int pid = fork();
    if (pid == -1)
    {
        std::cerr << "fork() failed for cgi file: " << response->GetFilePath() << std::endl;
        close(pipe_fd[0]), close(pipe_fd[1]);
        return ServerUtils::SendErrorResponse(
            HttpStatus(STATUS_INTERNAL_SERVER_ERROR, HTTP_STATUS_INTERNAL_SERVER_ERROR), response
        );
    }
    if (pid)
    {
        /*TODO: set response timeout*/
        close(pipe_fd[1]);
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
        close(STDIN_FILENO);
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
        /*inserting environment*/
        envp.insert(envp.end(), (char*)env_redirect.c_str());
        envp.insert(envp.end(), (char*)env_method.c_str());
        envp.insert(envp.end(), (char*)env_uri.c_str());
        envp.insert(envp.end(), (char*)env_query.c_str());
        envp.insert(envp.end(), (char*)env_script_name.c_str());
        envp.insert(envp.end(), NULL);

        execve(response->GetFileLocation()->cgi_path.c_str(), argv.data(), envp.data());
        std::cerr << "execve() failed for cgi file: " << response->GetFilePath() << std::endl;
        exit(10);
    }
}
