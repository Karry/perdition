/**********************************************************************
 * protocol.c                                            September 1999
 * Horms                                             horms@verge.net.au
 *
 * Generic protocol layer
 *
 * perdition
 * Mail retrieval proxy server
 * Copyright (C) 1999-2003  Horms
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

#include "pop3.h"
#include "pop3s.h"
#include "imap4.h"
#include "imap4s.h"
#include "protocol.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif


#ifdef WITH_SSL_SUPPORT
char *protocol_known[] = {"4", "POP3", "IMAP4", "POP3S", "IMAP4S"};
#else
char *protocol_known[] = {"2", "POP3", "IMAP4"};
#endif

/**********************************************************************
 * protocol_intitialise
 * initialise protocol structure
 * Pre: protocol_type: protocol type to use PROTOCOL_IMAP or PROTOCOL_POP3
 *      protocol: pointer to protocol strucure to be intialised
 * Post: protocol is intialised
 *       NULL on error
 **********************************************************************/

protocol_t *protocol_initialise(const int protocol_type, protocol_t *protocol){
  if((protocol=(protocol_t *)malloc(sizeof(protocol_t)))==NULL){
    VANESSA_LOGGER_DEBUG_ERRNO("malloc");
    return(NULL);
  }

  /* Seed the protocol structure with protocol specific values */
  switch (protocol_type){
    case PROTOCOL_POP3:
      if((protocol=pop3_initialise_protocol(protocol))==NULL){
        VANESSA_LOGGER_DEBUG("pop3_initialise_protocol");
	return(NULL);
      }
      break;
    case PROTOCOL_POP3S:
      if((protocol=pop3s_initialise_protocol(protocol))==NULL){
        VANESSA_LOGGER_DEBUG("pop3s_initialise_protocol");
	return(NULL);
      }
      break;
    case PROTOCOL_IMAP4:
      if((protocol=imap4_initialise_protocol(protocol))==NULL){
        VANESSA_LOGGER_DEBUG("imap4_initialise_protocol");
	return(NULL);
      }
      break;
    case PROTOCOL_IMAP4S:
      if((protocol=imap4s_initialise_protocol(protocol))==NULL){
        VANESSA_LOGGER_DEBUG("imap4s_initialise_protocol");
	return(NULL);
      }
      break;
    default:
      VANESSA_LOGGER_DEBUG("Unknown protocol");
      return(NULL);
  }

  return(protocol);
}


/**********************************************************************
 * protocol_destroy
 * destroy a protocol structure
 * Pre: protocol: allocated protocol structure
 * Return: none
 **********************************************************************/

void protocol_destroy(protocol_t *protocol){
  if(protocol==NULL){
    return;
  }

  /*Protocol specific destruction*/
  protocol->destroy(protocol);

  free(protocol);
  protocol=NULL;

  return;
}


/**********************************************************************
 * protocol_index
 * Return the index of a protocol in protocol_known
 * Pre: protocol_string: Protocol inn assci: IMAP4 or POP3
 *                       case insensitive
 * Post: Index of protocol in protocol_known
 *       0 if not found (unrecognised protocol)
 *       -1 on error
 **********************************************************************/

int protocol_index(const char *protocol_string){
  int i;

  extern char *protocol_known[];

  for(i=atoi(protocol_known[0]);i>0;i--){
    if(strcasecmp(protocol_string, protocol_known[i])==0){
      return(i);
    }
  }

  return(-1);
}


/**********************************************************************
 * protocol_list
 * List protocols in protocol_known
 * (known protocols)
 * Pre: string: pointer to  an unalocated string
 *      delimiter: delimiter to use in return string
 *      request: Index of protocol to list.
 *               List all protocols if 0
 * Post: string listing valid protocols
  *      NULL on error
 **********************************************************************/

char *protocol_list(char *string, const char *delimiter, const int request){
  int i;
  int noknown;
  size_t length;
  char *pos;
  char l;

  extern char *protocol_known[];

  noknown=atoi(protocol_known[0]);
  
  if((request<1 || request>noknown) && request!=PROTOCOL_ALL){
    VANESSA_LOGGER_DEBUG_UNSAFE("protocol \"%d\" out of range", request);
    return(NULL);
  }

  if(request!=PROTOCOL_ALL){
    return(strdup(protocol_known[request]));
  }

  /*extra 1 to allow space tor trailing '\0'*/
  length=1+(strlen(delimiter)*(noknown-1));

  for(i=noknown;i>0;i--){
    length+=strlen(protocol_known[i]);
  }

  if((string=(char *)malloc(length))==NULL){
    VANESSA_LOGGER_DEBUG_ERRNO("malloc");
    return(NULL);
  }

  pos=string;
  for(i=1;i<=noknown && length>0;i++){
    l=snprintf(pos, length, "%s", protocol_known[i]);
    pos+=l;
    length-=l;
    if(i<noknown && length>0){
      l=snprintf(pos, length, "%s", delimiter);
      pos+=l;
      length-=l;
    }
  }

  return(string);
}


char *protocol_capability(char *capability, flag_t flag,
		const char *existing_capability, const char *add_capability,
		const char *capability_delimiter) 
{
  char *tmp_str;
  
  if(!strcmp(PERDITION_PROTOCOL_DEPENDANT, capability)){
    tmp_str = strdup(existing_capability);
    if(tmp_str == NULL) {
      VANESSA_LOGGER_DEBUG_ERRNO("strdup");
      return(NULL);
    }
  }
  else {
    tmp_str = capability;
  }

  if(flag & PROTOCOL_C_ADD) {
    capability = str_append_substring_if_missing(tmp_str, add_capability, 
		    capability_delimiter);
    if(capability == NULL) {
      VANESSA_LOGGER_DEBUG("str_delete_substring");
      free(tmp_str);
      return(NULL);
    }
  }
  else {
    capability = str_delete_substring(tmp_str, add_capability, 
		    capability_delimiter);
    if(capability == NULL) {
      VANESSA_LOGGER_DEBUG("str_delete_substring");
      free(tmp_str);
      return(NULL);
    }
  }

  free(tmp_str);
  return(capability);
}
