/**********************************************************************
 * pop3s.h                                                   March 2002
 * Horms                                             horms@vergenet.net
 *
 * POP3S protocol defines
 *
 * perdition
 * Mail retrieval proxy server
 * Copyright (C) 1999-2002  Horms
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 *
 **********************************************************************/

#ifndef _PERDITION_POP3S_H
#define _PERDITION_POP3S_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "log.h"
#include "protocol_t.h"
#include "str.h"
#include "pop3_in.h"
#include "pop3_out.h"
#include "pop3_write.h"


/**********************************************************************
 * pop3s_intitialise_protocol
 * Intialialoise the protocol structure for the pop3s protocol
 * Pre: protocol: pointer to an allocated protocol structure
 * Post: Return seeded protocol stricture
 *              NULL on error
 **********************************************************************/

protocol_t *pop3s_initialise_protocol(protocol_t *protocol);


/**********************************************************************
 * pop3s_destroy_proto 
 * Destory protocol specifig elements of the protocol struture
 **********************************************************************/

void pop3s_destroy_protocol(protocol_t *protocol);


/**********************************************************************
 * pop3s_port 
 * Return the port to be used
 * pre: port: port that has been set
 * post: POP3S_DEFAULT_PORT if port is PERDITION_PROTOCOL_DEPENDANT
 *       port otherwise
 **********************************************************************/

char *pop3s_port(char *port);


/**********************************************************************
 * pop3s_encryption 
 * Return the encription states to be used.
 * pre: ssl_flags: the encryption flags that bave been set
 * post: If ssl_flags != SSL_MODE_EMPTY then return them,
 *       Else return SSL_MODE_SSL_ALL
 **********************************************************************/

flag_t pop3s_encryption(flag_t ssl_flags);

#endif
