/**********************************************************************
 * imap4.c                                               September 1999
 * Horms                                             horms@verge.net.au
 *
 * IMAP4 protocol defines
 *
 * perdition
 * Mail retrieval proxy server
 * Copyright (C) 1999-2005  Horms
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "imap4.h"
#include "protocol.h"
#include "options.h"
#include "perdition_globals.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif


static void imap4_destroy_protocol(protocol_t *protocol);
static char *imap4_port(char *port);
static flag_t imap4_encryption(flag_t ssl_flags);

/**********************************************************************
 * imap4_initialise_protocol
 * Initialise the protocol structure for the imap4 protocol
 * Pre: protocol: pointer to an allocated protocol structure
 * Post: Return seeded protocol stricture
 *              NULL on error
 **********************************************************************/

char *imap4_type[]={IMAP4_OK, IMAP4_NO, IMAP4_BAD};

protocol_t *imap4_initialise_protocol(protocol_t *protocol){
  protocol->type = imap4_type;
  protocol->write = imap4_write;
  protocol->greeting_string = IMAP4_GREETING;
  protocol->quit_string = IMAP4_QUIT;
  protocol->in_get_pw= imap4_in_get_pw;
#ifdef WITH_PAM_SUPPORT
  protocol->in_authenticate= imap4_in_authenticate;
#else
  protocol->in_authenticate= NULL;
#endif
  protocol->out_setup=imap4_out_setup;
  protocol->out_authenticate=imap4_out_authenticate;
  protocol->out_response=imap4_out_response;
  protocol->destroy = imap4_destroy_protocol;
  protocol->port = imap4_port;
  protocol->encryption = imap4_encryption;
  protocol->capability = imap4_capability;

  return(protocol);
}


/**********************************************************************
 * imap4_destroy_protocol 
 * Destroy protocol specific elements of the protocol structure
 **********************************************************************/

static void imap4_destroy_protocol(protocol_t *protocol)
{
  ;
}


/**********************************************************************
 * imap4_port 
 * Return the port to be used
 * pre: port: port that has been set
 * post: IMAP4_DEFAULT_PORT if port is PERDITION_PROTOCOL_DEPENDANT
 *       port otherwise
 **********************************************************************/

static char *imap4_port(char *port)
{
  if(!strcmp(PERDITION_PROTOCOL_DEPENDANT, port)){
    return(IMAP4_DEFAULT_PORT);
  }

  return(port);
}


/**********************************************************************
 * imap4_encryption 
 * Return the encryption states to be used.
 * pre: ssl_flags: the encryption flags that have been set
 * post: return ssl_flags (does nothing)
 **********************************************************************/

static flag_t imap4_encryption(flag_t ssl_flags) 
{
  return(ssl_flags);
}


/**********************************************************************
 * imap4_capability 
 * Return the capability string to be used.
 * pre: capability: capability string that has been set
 *      mangled_capability: not used
 *      tls_flags: the encryption flags that have been set
 *      tls_state: the current state of encryption for the session
 * post: capability to use, as per protocol_capability
 *       with IMAP4 parameters
 **********************************************************************/

char *imap4_capability(char *capability, char **mangled_capability,
		flag_t tls_flags, flag_t tls_state) 
{
	flag_t mode;

	if(!strcmp(capability, PERDITION_PROTOCOL_DEPENDANT)) {
		free(capability);
		capability = strdup(IMAP4_DEFAULT_CAPABILITY);
	}
  
      	if((tls_flags & SSL_MODE_TLS_LISTEN) && 
			!(tls_state & SSL_MODE_TLS_LISTEN)) {
		mode = PROTOCOL_C_ADD;
	}
	else {
		mode = PROTOCOL_C_DEL;
	}
	capability = protocol_capability(capability, mode, 
			capability, IMAP4_CMD_STARTTLS, 
			IMAP4_CAPABILITY_DELIMITER);
	if(capability == NULL) {
		VANESSA_LOGGER_DEBUG("protocol_capability");
		return(NULL);
	}

	if(!opt.login_disabled && (!(tls_flags & SSL_MODE_TLS_LISTEN) ||
			!(tls_flags & SSL_MODE_TLS_LISTEN_FORCE))) {
		return(capability);
	}

      	if(!(tls_state & SSL_MODE_TLS_LISTEN)) {
		mode = PROTOCOL_C_ADD;
	}
	else {
		mode = PROTOCOL_C_DEL;
	}
	capability = protocol_capability(capability, mode, 
			capability, IMAP4_CMD_LOGINDISABLED, 
			IMAP4_CAPABILITY_DELIMITER);
	if(capability == NULL) {
		VANESSA_LOGGER_DEBUG("protocol_capability");
		return(NULL);
	}

	return(capability);
}

