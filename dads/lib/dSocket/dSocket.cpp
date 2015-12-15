/**************************************************************************
DIVERSE DADS v1.0
released: April 7th
The DIVERSE HOMEPAGE is at: diverse.sourceforge.net

Copyright (C) 2000 - 2004  Andrew A. Ray, Patrick Shinpaugh, and Ron Kriz

This is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License (GPL) as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

This software is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this software, in the top level source directory in a file
named "COPYING.GPL"; if not, see it at:
http://www.gnu.org/copyleft/gpl.html or write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
USA.
**************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include "dSocket.h"
#include <errno.h>

#define BUFFER_SIZE 4096

dSocket::dSocket()
{
    m_closed = false;
    m_buffer = 0;
    m_bufsize = 0;
    resizeBuffer( BUFFER_SIZE );
	m_fd = -1;
	m_port = 0;
	m_type = 0;
	m_family = 0 ;
	m_protocol = 0;
	m_listening = false;
	memset( &m_sockaddress, '\0', sizeof( sockaddr_in) );
}

dSocket::dSocket( int fd, sockaddr_in sockaddress )
{
    m_closed = false;
    m_buffer = 0;
    m_bufsize = 0;
    resizeBuffer( BUFFER_SIZE );
	m_fd = fd;
	m_listening = false;
	m_sockaddress = sockaddress;
	m_port = m_sockaddress.sin_port;
	m_family = m_sockaddress.sin_family;
	determineNameAndAddress();
}

dSocket::dSocket( unsigned short port, int type, short family, int protocol )
{
    m_closed = false;
    m_buffer = 0;
    m_bufsize = 0;
    resizeBuffer( BUFFER_SIZE );
	m_port = port;
	m_type = type;
	m_family = family;
	m_protocol = protocol;
	if( ( m_fd = socket( family, type, protocol ) ) == -1 )
	{
	}
	m_sockaddress.sin_family = m_family;
	m_sockaddress.sin_port = htons( m_port );
	m_sockaddress.sin_addr.s_addr = INADDR_ANY;
	memset( &(m_sockaddress.sin_zero), '\0', 8 );
}

dSocket::dSocket( const string& address, unsigned short port, int type, short family,
		int protocol )
{
    m_closed = false;
    m_buffer = 0;
    m_bufsize = 0;
    resizeBuffer( BUFFER_SIZE );
	m_address = address;
	m_port = port;
	m_type = type;
	m_family = family;
	m_protocol = protocol;
	if( ( m_fd = socket( family, type, protocol ) ) == -1 )
	{
	}
	m_sockaddress.sin_family = m_family;
	m_sockaddress.sin_port = htons( m_port );
	m_sockaddress.sin_addr.s_addr = inet_addr( m_address.c_str() );
	memset( &(m_sockaddress.sin_zero), '\0', 8 );
}

dSocket::dSocket( unsigned short port, const string& name, int type, short family,
		int protocol )
{
    m_closed = false;
    m_buffer = 0;
    m_bufsize = 0;
    resizeBuffer( BUFFER_SIZE );
	m_name = name;
	m_port = port;
	m_type = type;
	m_family = family;
	m_protocol = protocol;
	if( ( m_fd = socket( family, type, protocol ) ) == -1 )
	{
	}
	m_sockaddress.sin_family = m_family;
	m_sockaddress.sin_port = htons( m_port );
	hostent* h;
	if( ( h = gethostbyname( m_name.c_str() ) ) == NULL )
	{
		herror( "gethostbyname" );
		((char*)m_sockaddress.sin_addr.s_addr)[0] = 127;
		((char*)m_sockaddress.sin_addr.s_addr)[1] = 0;
		((char*)m_sockaddress.sin_addr.s_addr)[2] = 0;
		((char*)m_sockaddress.sin_addr.s_addr)[3] = 1;
	}
	else if( h->h_addr == NULL )
	{
		((char*)m_sockaddress.sin_addr.s_addr)[0] = 127;
		((char*)m_sockaddress.sin_addr.s_addr)[1] = 0;
		((char*)m_sockaddress.sin_addr.s_addr)[2] = 0;
		((char*)m_sockaddress.sin_addr.s_addr)[3] = 1;
	}
	else
	{
		m_sockaddress.sin_addr = *((in_addr*)h->h_addr);
	}
	memset( &(m_sockaddress.sin_zero), '\0', 8 );
}

dSocket::~dSocket()
{
    if( m_buffer )
        free( m_buffer );
	closeSocket();
}

void dSocket::determineNameAndAddress()
{
	m_address = inet_ntoa( m_sockaddress.sin_addr );
	printf( "address   %s\n", m_address.c_str() );
	if( m_sockaddress.sin_addr.s_addr )
	{
		hostent* h;
		h = gethostbyaddr( &m_sockaddress.sin_addr.s_addr,
				sizeof( m_sockaddress.sin_addr.s_addr ), AF_INET );

		if( h )
		{
			printf( "name    %s\n", h->h_name );
			printf( "alias   %s\n", h->h_aliases[0] );
			printf( "address %s\n", h->h_addr_list[0] );
			m_name = string( h->h_name );
		}
		
	}
}

bool dSocket::resizeBuffer( int newsize )
{
    if( m_bufsize == newsize )
        return true;
    if( !( m_buffer = (char*)realloc( m_buffer, newsize ) ) )
    {
        printf( "ERROR: Unable to resize buffer\n" );
        free( m_buffer );
        m_buffer = 0;
        m_bufsize = 0;
        m_datasize = 0;
        return false;
    }
    m_bufsize = newsize;
    return true;
}

bool dSocket::createSocket()
{
	if( ( m_fd = socket( m_family, m_type, m_protocol ) ) == -1 )
	{
		perror( "socket" );
		return false;
	}
	return true;
}

bool dSocket::listenSocket( int backlog )
{
	if( listen( m_fd, backlog ) == -1 )
	{
		perror( "listen" );
		return false;
	}
	m_listening = true;
	return true;
}

bool dSocket::connectSocket()
{
	if( connect( m_fd, (sockaddr*)&m_sockaddress, sizeof(m_sockaddress) ) == -1 )
	{
		perror( "connect" );
		return false;
	}
	determineNameAndAddress();
	return true;
}

dSocket* dSocket::acceptSocket()
{
	sockaddr_in newsockaddr;
#ifdef DADS_ARCH_IRIX
	int len = sizeof(sockaddr_in);
#else
	socklen_t len = sizeof(sockaddr_in);
#endif
	int newfd;
	if( ( newfd = accept( m_fd, (sockaddr*)&newsockaddr, &len ) ) == -1 )
	{
		perror( "accept" );
		return NULL;
	}
	determineNameAndAddress();
	return new dSocket( newfd, newsockaddr );
}

bool dSocket::bindSocket()
{
	int yes = 1;
	if( setsockopt( m_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int) ) == -1 )
	{
		perror( "setsockopt" );
		return false;
	}
	if( bind( m_fd, (sockaddr*)&m_sockaddress, sizeof(m_sockaddress) ) == -1 )
	{
		perror( "bind" );
		return false;
	}
	return true;
}

bool dSocket::readSocket( int flags )
{
    int size = 0;
    m_datasize = 1;
    int total = 0;
    while( total < m_datasize )
    {
    	if( ( size = recv( m_fd, m_buffer + total, BUFFER_SIZE - 1, flags ) ) == -1 )
	    {
printf( "errno: %d\n", errno );
		    perror( "recv" );
    		return false;
	    }
    	else if( !size )
	    {
		    m_closed = true;
            printf( "Socket closed\n" );
    		return false;
	    }
        if( m_datasize == 1 )
        {
            int* temp = (int*)m_buffer;
            m_datasize = ntohl( *temp ) + 4;
            total = 0;
            if( m_bufsize < m_datasize + 1 )
            {
                if( !resizeBuffer( m_datasize + 1 ) )
                {
                    printf( "ERROR: Unable to resize buffer to %d bytes\n",
                            m_datasize + 1 );
                    return false;
                }
            }
        }
        total += size;
    }
   	m_buffer[m_datasize] = '\0';
	return true;
}

bool dSocket::writeSocket( const void* msg, int length, int flags )
{
	int size = 0;
    int total = 0;
    m_datasize = length + 4;
    if( m_bufsize < m_datasize + 1 )
    {
        if( !resizeBuffer( m_datasize + 1 ) )
        {
            printf( "ERROR: Unable to resize buffer to %d bytes\n",
                    m_datasize + 1 );
            return false;
        }
    }
    int* temp = (int*)m_buffer;
    *temp = htonl( length );
    memcpy( m_buffer + 4, msg, length );
    while( total < m_datasize )
    {
    	if( ( size = send( m_fd, m_buffer + total, m_datasize - total, flags ) ) == -1 )
	    {
		    perror( "send" );
	    	return false;
    	}
        total += size;
    }
    m_datasize = 0;
    m_buffer[0] = '\0';
	return true;
}

bool dSocket::closeSocket()
{
	if( close( m_fd ) == -1 )
		return false;
	m_fd = -1;
	m_port = 0;
	m_type = 0;
	m_family = 0 ;
	m_protocol = 0;
	m_name.clear();
	m_address.clear();
	m_closed = true;
	memset( &m_sockaddress, '\0', sizeof( sockaddr_in) );
	return true;
}

