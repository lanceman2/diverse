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
#ifndef _DSOCKET_H_
#define _DSOCKET_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

using namespace std;

class dSocket
{
public:
	// Everything will be set manually and created manually
	dSocket();
	// Assumes socket already exists
	dSocket( int fd, sockaddr_in address );
	// Will create socket for server side automatically determining the address
	dSocket( unsigned short port, int type = SOCK_STREAM, short family = AF_INET,
			int protocol = 0 );
	// Will create socket with address in IP format
	dSocket( const string& address, unsigned short port, int type = SOCK_STREAM,
			short family = AF_INET, int protocol = 0 );
	// Will create socket using host name
	dSocket( unsigned short port, const string& name, int type = SOCK_STREAM,
			short family = AF_INET, int protocol = 0 );
	~dSocket();

	inline int getfd() { return m_fd; };
	sockaddr_in getSocketAddress() { return m_sockaddress; };

	// Functions to be used when setting up a socket which is not currently open
	void setAddress( const string& address ) { m_address = address; };
	void setPort( unsigned short port ) { m_port = port; };
	void setType( int type ) { m_type = type; };
	void setFamily( short family ) { m_family = family; };
	void setProtocol( int protocol ) { m_protocol = protocol; };
	// Functions to get information about the socket
	string getName() { return m_name; };
	string getAddress() { return m_address; };
	unsigned short getPort() { return m_port; };
	int getType() { return m_type; };
	short getFamily() { return m_family; };
	int getProtocol() { return m_protocol; };

	void* getBuffer() { return m_buffer ? m_buffer + 4 : 0; };
	int getSize() { return m_datasize ? m_datasize - 4 : 0; };

	// create the socket if it does not exist
	bool createSocket();
	// listen on a socket
	bool listenSocket( int backlog = 6 );
	// used to connect to other machine
	bool connectSocket();
	// used to accept connections when listening
	dSocket* acceptSocket();
	// used on server side to bind socket to local machine
	bool bindSocket();
	// read from existing connection
	bool readSocket( int flags = 0 );
	// write to existing connection
	bool writeSocket( const void* msg, int length, int flags = 0 );
//	// read from connection (UDP only)
//	bool readFromSocket();
//	// write to connection (UDP only)
//	bool writeToSocket( const char* msg );
	// close the socket - can be reused if desired as all data is cleared
	bool closeSocket();
	bool isClosed() { return m_closed; };

private:
	// Utility function to find name and address socket is connected to
	void determineNameAndAddress();
    // Resize the buffer
    bool resizeBuffer( int newsize );
	// file descriptor
	int m_fd;
	// socket connection address
	sockaddr_in m_sockaddress;
	string m_name;
	string m_address;
	unsigned short m_port;
	int m_type;
	short m_family;
	int m_protocol;
	bool m_listening;
    int m_datasize;
	int m_bufsize;
//	char m_buffer[1024];
    char* m_buffer;
	bool m_closed;
//	int m_index;
};

#endif

