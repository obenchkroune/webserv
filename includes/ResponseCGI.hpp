/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseCGI.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 00:58:15 by simo              #+#    #+#             */
/*   Updated: 2025/01/26 18:17:45 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "ServerUtils.hpp"
#include <cassert>
#include <cstring>

class ResponseCGI : public Response
{
public:
    virtual ~ResponseCGI();
    ResponseCGI(const Response& response);

private:
    ResponseCGI(const ResponseCGI& responseCGI);
    ResponseCGI& operator=(const ResponseCGI& responseCGI);

public:
    virtual void FinishResponse(bool append_content_length = true);
};