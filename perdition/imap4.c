/**********************************************************************
 * imap4.c                                               September 1999
 * Horms                                             horms@vergenet.net
 *
 * IMAP4 protocol defines
 *
 * perdition
 * Mail retrieval proxy server
 * Copyright (C) 1999  Horms
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

#include "imap4.h"

/**********************************************************************
 * imap4_intitialise_proto
 * Intialialoise the protocol structure for the imap4 protocol
 * Pre: protocol: pointer to an allocated protocol structure
 * Post: Return seeded protocol stricture
 *              NULL on error
 **********************************************************************/

char *imap4_type[]={IMAP4_OK, IMAP4_NO, IMAP4_BAD};

protocol_t *imap4_initialise_protocol(protocol_t *protocol){
  extern char *imap4_type[];

  protocol->type = imap4_type;
  protocol->write = imap4_write;
  protocol->greeting_string = IMAP4_GREETING;
  protocol->quit_string = IMAP4_QUIT;
  protocol->one_time_tag = IMAP4_ONE_TIME_TAG;
  protocol->in_get_pw= imap4_in_get_pw;
  protocol->out_authenticate=imap4_out_authenticate;
#ifdef WITH_PAM_SUPPORT
  protocol->in_authenticate= imap4_in_authenticate;
#else
  protocol->in_authenticate= NULL;
#endif
  protocol->out_response=imap4_out_response;
  protocol->destroy = imap4_destroy_protocol;
  protocol->port = imap4_port;

  return(protocol);
}


/**********************************************************************
 * imap4_destroy_proto 
 * Destory protocol specifig elements of the protocol struture
 **********************************************************************/

void imap4_destroy_protocol(protocol_t *protocol){
  ;
}


/**********************************************************************
 * imap4_port 
 * Return the port to be used
 * pre: port: port that has been set
 * post: IMAP4_DEFAULT_PORT if port is PERDITION_PROTOCOL_DEPENDANT
 *       port otherwise
 **********************************************************************/

char *imap4_port(char *port){
  if(!strcmp(PERDITION_PROTOCOL_DEPENDANT, port)){
    return(IMAP4_DEFAULT_PORT);
  }

  return(port);
}

