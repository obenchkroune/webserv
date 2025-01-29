/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shared.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/01 21:09:07 by msitni1337        #+#    #+#             */
/*   Updated: 2025/01/29 02:22:23 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define PROGVERSION          "0.2"
#define PROGNAME             "webserv"
#define READ_CHUNK           1024 * 4UL
#define RECV_CHUNK           1024UL
#define SEND_CHUNK           1024UL
#define REQUEST_HEADER_LIMIT RECV_CHUNK * 100UL