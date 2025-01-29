/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseCGI.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 00:58:15 by simo              #+#    #+#             */
/*   Updated: 2025/01/29 00:44:56 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "ServerUtils.hpp"
#include <cassert>
#include <cstring>

class ResponseCGI : public Response
{
private:
    HttpStatus  _cgi_status;
    const char* _cgi_line_delimiter;
    bool        _cgi_is_reading_body;

public:
    virtual ~ResponseCGI();
    ResponseCGI(const Response& response);

private:
    ResponseCGI(const ResponseCGI& responseCGI);
    ResponseCGI& operator=(const ResponseCGI& responseCGI);

public:
    virtual void AppendToResponseBuff(const std::vector<uint8_t>& content);
    virtual void FinishResponse();
};