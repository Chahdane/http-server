#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <istream>
#include <iostream>
#include <unistd.h>
#include <string>  
#include <fcntl.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <arpa/inet.h>

class Socket
{
    public :
        Socket(std::string port, std::string ip);
		int getSocket();
    private :
        int _socket;
};

#endif