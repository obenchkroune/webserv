/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shared.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/01 21:09:07 by msitni1337        #+#    #+#             */
/*   Updated: 2025/01/27 19:54:07 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define PROGVERSION          "0.1"
#define PROGNAME             "webserv"
#define READ_CHUNK           1024 * 4UL
#define RECV_CHUNK           1024UL
#define SEND_CHUNK           1024UL
#define REQUEST_HEADER_LIMIT RECV_CHUNK * 100UL