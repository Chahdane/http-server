#include "Socket.hpp"
#include "../WebServer/WebServer.hpp"

 Socket::Socket(std::string port, std::string ip) {
    int port_int = stoi(port);
	int val = 1;
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port_int);
    if((_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		pexit("socket error:");
    fcntl(_socket, F_SETFL, O_NONBLOCK);
    if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == -1)
        pexit("setsocketopt :");
    address.sin_addr.s_addr = inet_addr(ip.c_str());
    if((bind(_socket, (struct sockaddr *)&address, sizeof(address))) < 0)
        pexit("binding error:");
    if (listen(_socket, 80) != 0)
		pexit("listening error:");
    std::cout << "------- " << port << "-------" <<std::endl;
}

int Socket::getSocket()
{
	return _socket; 
}