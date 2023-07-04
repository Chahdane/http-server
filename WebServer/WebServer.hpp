#pragma once
#include <sys/types.h>
#include <istream> 
#include <fcntl.h>
#include <stdio.h>
#include <iostream>
#include <netdb.h>
#include <map>
#include <sys/stat.h>
#include <dirent.h>
#include "../Parsing/Parsing.hpp"
#include "WebServer.hpp"
#include "../Socket/Socket.hpp"

class ClientSocket;

class WebServ
{
    public :
        void run();
        void setupStatusCodes();
        void clientsQueue();
        void createClients();
        void manageClients();
		int  sendToClient(ClientSocket &client, char *msg, int size);
		void sendToClient(ClientSocket &client, std::string msg);
        void sendResponse(ClientSocket client, std::string dir, int code);
        void sendErrorToClient(int err, ClientSocket &client);
        bool isAllowed(std::vector<std::string> methodlist, std::string methodreq);
        void autoIndex(int socket, std::string path, std::string fullurl, ClientSocket &client);
        bool eraseClient(ClientSocket client);
        void setToWait(int socket, fd_set &set);
        void selection(fd_set &read, fd_set &write);
        bool selectAndWrite(std::string url, ClientSocket client, std::string str, Request req);
		void clientError(int err, ClientSocket &client);
		int checkRequest(Request &request, ClientSocket &client, int size);
        std::string fixUrl(std::string url, int i);
        Location *getLocation(std::string url, int i);
		void runMethods(ClientSocket &client, std::string url, Request &request);
        void GET(ClientSocket &client, std::string url);
        void DELETE(ClientSocket &client, std::string url);
        void POST();
        void redirect(ClientSocket client, std::string url);
        std::vector<Socket> getSocketList() {return sockets; }
        std::vector<ClientSocket> getClientsList() {return clients; }
		WebServ(char **envp , std::vector<Server*> servers);
        std::vector<Server*> servers;
        void parseMimeTypes(void);
        void printMimeTypes(void);
        std::string getMimeType(std::string &extension);
        std::string getExtensionFromUrl(const std::string& url);
        std::string getMimeTypeFromExtension(const std::string& url);
        char **envp;
        int maxFds;
        fd_set readingSet;
        fd_set writingSet;
        Location *locations;
        std::string queryString;
    private :
        std::map<std::string, std::string> _mimeTypes;
        std::vector<Socket> sockets;
        std::vector<ClientSocket> clients;
        std::map<int , std::string> error_pages;
};


class ClientSocket
{
    private :
        int clientSocket;
        int serverId;
    public :
        int getClientSocket() { return clientSocket; }
        int getServerID() { return serverId; }
        void setServerId(int n) { serverId = n; }
        int size;
        char buffer[65537];
        std::string request;
		ClientSocket(int sock, int id)
		{
			clientSocket = sock;
			fcntl(clientSocket, F_SETFL, O_NONBLOCK);
			serverId = id;
			bzero(buffer, 65537);
			size = 0;
		}
};

void pexit(std::string err);
std::string trimString(const std::string& str);
