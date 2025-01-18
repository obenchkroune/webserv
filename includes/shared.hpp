/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shared.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/01 21:09:07 by msitni1337        #+#    #+#             */
/*   Updated: 2024/12/10 17:08:49 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define PROGVERSION          "0.1"
#define PROGNAME             "webserv"
#define READ_CHUNK           1024 * 4
#define RECV_CHUNK           1024
#define SEND_CHUNK           1024
#define REQUEST_HEADER_LIMIT RECV_CHUNK * 100UL