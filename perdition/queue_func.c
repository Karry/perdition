/**********************************************************************
 * queue_func.c                                            October 1999
 * Horms                                             horms@vergenet.net
 *
 * Functions build around the queue ADT
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

#include "queue_func.h"


/**********************************************************************
 * read_line
 * read a line from fd and parse it into a queue of tokens
 * line is read by making repeated calls to read_token
 * pre: fd: file descriptor to read from
 *      buf: buffer to store bytes read from server in
 *      n: pointer to size_t containing the size of literal_buf
 * post: Token is read from fd into token
 *       If literal_buf is not NULL, and n is not NULL and *n is not 0
 *       Bytes read from fd are copied to literal_buf.
 * return: token
 *         NULL on error
 * Note: If buf is being filled and space is exausted function will
 *       return what has been read so far. (No buffer overflows today)
 **********************************************************************/

vanessa_queue_t *read_line(const int fd, unsigned char *buf, size_t *n){
  token_t *t=NULL;
  size_t buf_offset=0;
  size_t buf_remaining;
  vanessa_queue_t *q;
  int do_literal;

  if(buf!=NULL && n!=NULL && *n!=0){
    do_literal=1;
    buf_remaining=*n;
  }
  else{
    buf=NULL;
    buf_remaining=0;
    do_literal=0;
  }

  if((q=vanessa_queue_create(DESTROY_TOKEN))==NULL){
    PERDITION_LOG(LOG_DEBUG, "read_line: create_queue");
    return(NULL);
  }
 
  do{
    if((t=read_token(fd,(buf==NULL)?NULL:buf+buf_offset,&buf_remaining))==NULL){
      PERDITION_LOG(LOG_DEBUG, "read_line: read_token");
      vanessa_queue_destroy(q);
      return(NULL);
    }

    if(do_literal){
      buf_offset+=buf_remaining;
      buf_remaining=*n-buf_offset;
    }

    if((q=vanessa_queue_push(q, (void *)t))==NULL){
      PERDITION_LOG(LOG_DEBUG, "read_line: vanessa_queue_push");
      return(NULL);
    }
  }while(!token_is_eol(t) && !(do_literal && buf_offset>=*n));

  if(do_literal){
    *n=buf_offset;
  }

  return(q);
}


/**********************************************************************
 * queue_to_string
 * convert the contents of a queue of tokens into a string
 * a space ( ) is inserted in the resultant string between each
 * token
 * pre: q queue to dump as a string
 * post: a string is allocated and the quie is dumped to the string
 *       the string is '\0' terminated
 * return: allocated string
 *         NULL on error
 **********************************************************************/

char *queue_to_string(vanessa_queue_t *q){
  token_t *t;
  vanessa_queue_t *stack=NULL;
  size_t length=0;
  char *string;
  char *pos;

  if((stack=vanessa_queue_create(DESTROY_TOKEN))==NULL){
    PERDITION_LOG(LOG_DEBUG, "queue_to_string: create_queue");
    return(NULL);
  }

  while(q->first!=NULL){
    if((q=vanessa_queue_pop(q, (void **)&t))==NULL){
      PERDITION_LOG(LOG_DEBUG, "queue_to_string: vanessa_queue_pop 1");
      return(NULL);
    }

    length+=1+t->n;

    if((stack=vanessa_queue_push(stack, (void *)t))==NULL){
      PERDITION_LOG(LOG_DEBUG, "queue_to_string: vanessa_queue_push");
      return(NULL);
    }
  }

  vanessa_queue_destroy(q);

  if((string=(char*)malloc(sizeof(char)*length))==NULL){
    PERDITION_LOG(LOG_DEBUG, "queue_to_string: malloc");
    vanessa_queue_destroy(stack);
    return(NULL);
  }

  pos=string;
  while(stack->first!=NULL){
    if((stack=vanessa_queue_pop(stack, (void **)&t))==NULL){
      PERDITION_LOG(LOG_DEBUG, "queue_to_string: vanessa_queue_pop 2");
      free(string);
      return(NULL);
    }
    
    strncpy(pos, t->buf, t->n);
    pos+=t->n;
    *pos++=' ';
  }
  
  vanessa_queue_destroy(stack);
  *--pos='\0';

  return(string);
}
