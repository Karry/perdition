/**********************************************************************
 * io.c                                                        May 2001
 * Horms                                             horms@vergenet.net
 *
 * Abstraction layer to allow I/O to file descriptors or SSL objects
 *
 * perdition
 * Mail retrieval proxy server
 * Copyright (C) 1999-2001  Horms
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


#ifndef IO_FRUB_H
#define IO_FRUB_H

typedef void io_t;


/**********************************************************************
 * io_create_fd 
 * Create an io that uses fds
 * pre: read_fd: File descriptor for reading
 *      write_fd: File descriptor for writing
 * post: io_t is intialised to use fds
 * return: new io_t
 *         NULL on error
 **********************************************************************/

io_t *io_create_fd(int read_fd, int write_fd);


#ifdef WITH_SSL_SUPPORT
#include <openssl/ssl.h>

/**********************************************************************
 * io_create_ssl 
 * Create an io that uses ssl
 * pre: ssl: SSL object
 *      read_fd: File descriptor for reading from
 *      write_fd: File descriptor for writing to
 * post: io_t is intialised to use ssl
 * return: new io_t
 *         NULL on error
 **********************************************************************/

io_t *io_create_ssl(
  SSL *ssl, 
  int read_fd, 
  int write_fd
);
#endif /* WITH_SSL_SUPPORT */


/**********************************************************************
 * io_destroy
 * Destroy and io_t
 * If it is an SSL io_t then call SSL_free()
 * pre: io: io_t to free
 * post: io and any internal data is freed
 * return: none
 **********************************************************************/

void io_destroy(io_t *io);


/**********************************************************************
 * io_write
 * Write to an io_t
 * pre: io: io_t to write to
 *      buf: buffer to write
 *      count: number of bytes to write
 * return: Number of bytes written
 *         -1 on error
 **********************************************************************/

ssize_t io_write(
  io_t *io, 
  const void *buf, 
  size_t count
);


/**********************************************************************
 * io_read
 * Read from an io_t
 * pre: io: io_t to read from
 *      buf: buffer to read
 *      count: maximum number of bytes to read
 * return: Number of bytes read
 *         -1 on error
 **********************************************************************/


ssize_t io_read(
  io_t *io, 
  void *buf, 
  size_t count
);


/**********************************************************************
 * io_get_rfd
 * Get the file descriptor that is being used for reading
 * pre: io: io_t to get read file descriptor of
 * return: file descriptor
 *         -1 on error
 **********************************************************************/

int io_get_rfd(io_t *io);


/**********************************************************************
 * io_get_wfd
 * Get the file descriptor that is being used for writing
 * pre: io: io_t to get write file descriptor of
 * return: file descriptor
 *         -1 on error
 **********************************************************************/

int io_get_wfd(io_t *io);


#ifdef WITH_SSL_SUPPORT
/**********************************************************************
 * io_get_ssl
 * Get ssl object for io, if there is one
 * pre: io: io_t to get the ssl object of
 * return: ssl object descriptor
 *         NULL on error if if there is no ssl object for this io
 **********************************************************************/

SSL *io_get_ssl(io_t *io);
#endif /* WITH_SSL_SUPPORT */


/**********************************************************************
 * io_close
 * Close the file descriptors in an io_t
 * If it is an SSL io_t then call SSL_shutdown();
 * pre: io: io_t close the file descriptors of
 * return: 0 on success
 *         -1 on error
 **********************************************************************/

int io_close(io_t *io);


/**********************************************************************
 * io_pipe
 * pipe bytes from from one io_t to another ance vice versa
 * pre: io_a: one of the io_t
 *      io_b: the other io_t
 *      buffer:   allocated buffer to read data into
 *      buffer_length: size of buffer in bytes
 *      idle_timeout:  timeout in seconds to wait for input
 *                     timeout of 0 = infinite timeout
 *      return_a_read_bytes: Pointer to int where number
 *                           of bytes read from a will be recorded.
 *      return_b_read_bytes: Pointer to int where number
 *                           of bytes read from b will be recorded.
 * bytes are read from io_a and written to io_b and vice versa
 * return: -1 on error
 *         1 on idle timeout
 *         0 otherwise (one of io_a or io_b closes gracefully)
 *
 **********************************************************************/


ssize_t io_pipe(
  io_t *io_a,
  io_t *io_b,
  unsigned char *buffer,
  int buffer_length,
  int idle_timeout,
  int *return_a_read_bytes,
  int *return_b_read_bytes
);

#endif