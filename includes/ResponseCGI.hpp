/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseCGI.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 00:58:15 by simo              #+#    #+#             */
/*   Updated: 2025/01/08 16:41:14 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "ServerUtils.hpp"
#include <cassert>
#include <cstring>

class ResponseCGI : public Response
{
private:
    ResponseCGI(const Request& request, const ServerConfig& virtual_server, Server* server);

public:
    virtual ~ResponseCGI();
    ResponseCGI(const Response& responseCGI);

private:
    ResponseCGI& operator=(const ResponseCGI& responseCGI);

public:
    virtual void FinishResponse(bool append_content_length = true);
};