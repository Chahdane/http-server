#include "../WebServer/WebServer.hpp"
#include "../Parsing/Request.hpp"

WebServ::WebServ(char **envp , std::vector<Server*> servers)
{
    this->servers = servers;
    this->envp = envp;
    parseMimeTypes();
    for(size_t i = 0; i < servers.size(); i++)
    {
        Socket socket(servers[i]->getListen(), servers[i]->getName());
        sockets.push_back(socket);
    }
	setupStatusCodes();
}

void WebServ::setupStatusCodes()
{
    std::ifstream file("status.codes");

    if (!file.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        int StatusCode;
        std::string error;
        std::istringstream ss(line);
        if (!(ss >> StatusCode))
        {
            continue;
        }
        std::getline(ss, error);

        error = error.substr(error.find_first_not_of(' '));
        error = error.substr(0, error.find_last_not_of(' ') + 1);

        if (!error.empty())
            error_pages.insert(std::make_pair(StatusCode, error));
    }
    file.close();
    maxFds = -1;
}


bool WebServ::eraseClient(ClientSocket client)
{
    close(client.getClientSocket());
    for(size_t i = 0; i < clients.size(); i++)
    {
        if(clients[i].getClientSocket() == client.getClientSocket())
        {
            clients.erase(clients.begin() + i);
            return true;
        }
    }
    exit(1);
}

bool WebServ::isAllowed(std::vector<std::string> methods, std::string method)
{
    for(size_t i = 0; i < methods.size(); i++)
        if(methods[i] == method)
            return true;
    return false;
}

void WebServ::sendToClient(ClientSocket &client, std::string msg)
{	
	int ret;
	if((ret = send(client.getClientSocket(), msg.c_str(), msg.size(), 0)) < 0)
        sendErrorToClient(500, client);
    else if(ret == 0)
        sendErrorToClient(400, client);
}

int WebServ::sendToClient(ClientSocket &client, char *msg, int size)
{	
	int ret;
	if((ret = send(client.getClientSocket(), msg, size, 0)) < 0)
        sendErrorToClient(500, client);
    else if(ret == 0)
        sendErrorToClient(400, client);
	return ret;
}



void WebServ::sendResponse(ClientSocket client, std::string dir, int code)
{	
    if(!dir.empty())
    {
        FILE *fd_s = fopen(dir.c_str(), "rb");
		if (fd_s == NULL)
			sendToClient(client ,  "HTTP/1.1 200\ret\nConnection: close\ret\nContent-Length: 120\ret\n\ret\n<!DOCTYPE html><head><title>makaynach</title></head><h1>wa hya location kayna walakin fin l file?</h1><body> </body></html>");
		else
		{
			fseek (fd_s , 0, SEEK_END);
			int lSize = ftell (fd_s);
			rewind (fd_s);
			fclose(fd_s);
			sendToClient(client,"HTTP/1.1 " + std::to_string(code) + " " + this->stringifyError(code) + "\r\n" + "Content-Type: " + getMimeTypeFromExtension(dir) + "\r\nContent-Length: " + std::to_string(lSize) + "\r\n\r\n");
			int readFD = open(dir.c_str(), O_RDONLY);
			if (readFD < 0) return (sendErrorToClient(500, client));

			char file[1024];
			int ret = read(readFD, file, 1024);

            int ret2;


			if(ret < 0)
				sendErrorToClient(500, client);
			else 
			{
				while(ret)
				{
                    ret2 = send(client.getClientSocket(), file, ret, 0);

					if((ret = read(readFD, file, 1024)) < 0)
					{
						sendErrorToClient(500, client);
						break;
					}
					if(ret == 0)
                    {
                        break;
                    }
						
				}
			}
			close(readFD);
		}
    }
    return ;
	sendToClient(client ,  "HTTP/1.1 " + error_pages[code] + "\n\n");
}

std::string WebServ::fixUrl(std::string url, int serverID)
{
    std::string ret = servers[serverID]->getRoot();
    
    if (ret.back() == '/')
        ret.pop_back();
    
    if (locations && !locations->getRoot().empty())
    {
        size_t dirPos = url.find(locations->getDir());
        if (dirPos != std::string::npos)
            url.erase(dirPos, locations->getDir().size());
    }
    
    return ret + url;
}


void WebServ::autoIndex(int socket, std::string path, std::string url, ClientSocket& client)
{
    Location *location = getLocationByUrl(path, client);
    if (location && !location->getCgi().empty())
        return ;
    struct dirent *ent;
    std::string tosend = "HTTP/1.1 200 OK\n\n<!DOCTYPE html>\n<html>\n<body>\n<h1>" + path + "</h1>\n<pre>\n";
	DIR *dir = opendir (url.c_str());
	if (!dir) {
        perror ("autoindex");
        return ;
    }
    if(path[path.size() - 1] == '/')
        path.erase(path.size() - 1, 1);
    while ((ent = readdir (dir)) != NULL)
    {
        std::string type = "file";
        if (ent->d_type == DT_DIR)
            type = "dir";
        else if (ent->d_type == DT_LNK)
            type = "link";
        else if (ent->d_type == DT_UNKNOWN)
            type = "unknown";
        tosend += "<a href=\"" + ((std::string(ent->d_name) == ".") ? std::string(path) : (std::string(path) + "/" + std::string(ent->d_name))) + (type == "dir" ? "/" : "") + "\">" + std::string(ent->d_name) + (type == "dir" ? "/" : "") + "</a> (" + type + ")\n";
    }
    closedir (dir);
    tosend += "</pre>\n</body>\n</html>\n";
    int ret = send(socket , tosend.c_str(), tosend.size(), 0);
    if(ret < 0)
        sendErrorToClient(500, client);
    else if (ret == 0)
        sendErrorToClient(400, client);
}

Location *WebServ::getLocation(std::string url, int i)
{
    std::vector<Location *> locations = servers[i]->getLocation();
    for(size_t i = 0; i < locations.size(); i++)
    {
        if(strncmp(locations[i]->getDir().c_str(), url.c_str(), locations[i]->getDir().size()) == 0)
            return locations[i];
    }
    return NULL;
}

Location *WebServ::getLocationByUrl(std::string url, ClientSocket client)
{

    std::vector<Location *> locations = servers[client.getServerID()]->getLocation();
    for(size_t i = 0; i < locations.size(); i++)
    {
        if (url == locations[i]->getDir() + "/" || url == locations[i]->getDir())
        {
            return locations[i];
        }    
    }
    return NULL;
}

std::string WebServ::getLocationRoot(std::string url, ClientSocket client)
{
    std::vector<Location *> locations = servers[client.getServerID()]->getLocation();
    for(size_t i = 0; i < locations.size(); i++)
        if (url == locations[i]->getDir() + "/" || url == locations[i]->getDir())
            return locations[i]->getRoot().erase(0,1);
    return "none";
}

bool WebServ::isValidLoc(std::string url, ClientSocket client)
{
    if (url == "/")
        return true;
    std::vector<Location *> locations = servers[client.getServerID()]->getLocation();
    for(size_t i = 0; i < locations.size(); i++)
        if (url == locations[i]->getDir() + "/" || url == locations[i]->getDir())
            return true;
    return false;
}



void WebServ::redirect(ClientSocket client, std::string url)
{
    std::string tosend = "HTTP/1.1 301 Moved Permanently\r\n\r\n<head><meta http-equiv = \"refresh\" content = \"0; url =" + url + "\" /></head>";
    int ret = send(client.getClientSocket(), tosend.c_str(), tosend.size(), 0);
	if (ret > 0) return ;
	(ret < 0) ? sendErrorToClient(500, client) : sendErrorToClient(400, client);
}

std::string WebServ::stringifyError(int err)
{
    static std::map<int, std::string> errors;
    if (errors.empty())
    {
        errors[100] = "Continue";
        errors[101] = "Switching Protocols";
        errors[200] = "OK";
        errors[201] = "Created";
        errors[202] = "Accepted";
        errors[203] = "Non-Authoritative Information";
        errors[204] = "No Content";
        errors[205] = "Reset Content";
        errors[206] = "Partial Content";
        errors[300] = "Multiple Choices";
        errors[301] = "Moved Permanently";
        errors[302] = "Found";
        errors[303] = "See Other";
        errors[304] = "Not Modified";
        errors[305] = "Use Proxy";
        errors[307] = "Temporary Redirect";
        errors[400] = "Bad Request";
        errors[401] = "Unauthorized";
        errors[402] = "Payment Required";
        errors[403] = "Forbidden";
        errors[404] = "Not Found";
        errors[405] = "Method Not Allowed";
        errors[406] = "Not Acceptable";
        errors[407] = "Proxy Authentication Required";
        errors[408] = "Request Time-out";
        errors[409] = "Conflict";
        errors[410] = "Gone";
        errors[411] = "Length Required";
        errors[412] = "Precondition Failed";
        errors[413] = "Request Entity Too Large";
        errors[414] = "Request-URI Too Large";
        errors[415] = "Unsupported Media Type";
        errors[416] = "Requested range not satisfiable";
        errors[417] = "Expectation Failed";
        errors[500] = "Internal Server Error";
        errors[501] = "Not Implemented";
        errors[502] = "Bad Gateway";
        errors[503] = "Service Unavailable";
        errors[504] = "Gateway Time-out";
        errors[505] = "HTTP Version not supported";
    }
    if (errors.find(err) == errors.end())
        return "Something went wrong";
    return errors[err];
}

void WebServ::sendErrorToClient(int err, ClientSocket &client)
{
    std::map<std::string , std::string> errpages = this->servers[client.getServerID()]->getError();
    if(errpages.find(std::to_string(err)) != errpages.end())
    {
        int fd = open(errpages[std::to_string(err)].c_str(), O_RDONLY);
        if(fd >= 0)
            return (close(fd), this->sendResponse(client, errpages[std::to_string(err)], err));
    }
    std::string errorPage = "<html><head><title>" + std::to_string(err) + " " + this->stringifyError(err) + "</title><style>*{font-family:\"Arial\";}</style></head><body><center><br/><h1>" + std::to_string(err) + " - " + this->stringifyError(err) + "</h1><hr/></center></body></html>";
    std::string response = "HTTP/1.1 " + std::to_string(err) + " " + this->stringifyError(err) + "\r\n"
                        "Content-Type: text/html\r\n"
                        "Content-Length: " + std::to_string(errorPage.length()) + "\r\n\r\n" +
                        errorPage;
    this->sendToClient(client, response);
}

bool WebServ::selectAndWrite(std::string url, ClientSocket client, std::string str, Request req)
{
    int ret = 0;
    int fd = open(url.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if(fd < 0)
    {
        sendErrorToClient(500, client);
        close(fd);
        return false;
    }
	(str == "") ? ret = write(fd, req.getBody().c_str(), req.getBody().size()) : ret = write(fd, str.c_str(), str.size());
    if(ret < 0)
    {
        sendErrorToClient(500, client);
        close(fd);
        return false;
    }
    close(fd);
    return true;
}

void WebServ::setToWait(int socket, fd_set &set)
{
    FD_SET(socket, &set);
    if(socket > maxFds)
        maxFds = socket;
}

void WebServ::selection(fd_set &read, fd_set &write)
{
    if((select(maxFds + 1, &read, &write, 0, 0)) < 0)
        exit(-1);
    writingSet = write;
    readingSet = read;
}

void WebServ::clientsQueue()
{
    fd_set read, write;
    FD_ZERO(&read);
    FD_ZERO(&write);
	size_t i = 0;
	while (i < sockets.size())
	{
		setToWait(sockets[i].getSocket(), read);
		if (i < clients.size())
			setToWait(clients[i].getClientSocket(), read);
		i++;
	}
}

void WebServ::createClients()
{
    sockaddr_in addrclient;
    socklen_t clientSize = sizeof(addrclient);
	size_t i = 0;

	while (i < sockets.size())
	{
		if(FD_ISSET(sockets[i].getSocket(), &readingSet))
        {
			int addClient = accept(sockets[i].getSocket(), (struct sockaddr *)&addrclient, &clientSize);
            if(addClient < 0)
				pexit("accept: ");
			ClientSocket client(addClient, i);
			clients.push_back(client);
        }
		i++;
	}
}

void WebServ::clientError(int err, ClientSocket &client)
{
	sendErrorToClient(err, client);
    eraseClient(client);
}

int WebServ::checkRequest(Request &request, ClientSocket &client, int size)
{
    if (size < 0)
    {
        clientError(413, client);
        return 1;
    }
	if ((request.getMethod() != "POST" && request.getMethod() != "GET" && request.getMethod() != "DELETE"))
    {
        clientError(405, client);
        return 1;       
    }
	if (request.getProtocol() != "HTTP/1.1")
	{
		clientError(505, client);
        return 1;    
	}
    if(request.getContentLength() != std::string::npos && request.getContentLength() > (size_t)stoi(servers[client.getServerID()]->getBody()))
    {
        clientError(413, client);
        return 1;
    } 
	return 0;
}

bool WebServ::isAllowdMethod(ClientSocket &client, std::string method, std::string url)
{
    if (!isValidLoc(url, client))
        return true;
    if (url == "/")
    {
        std::vector<std::string> methods = servers[client.getServerID()]->getMethod();
        for(size_t i = 0; i < methods.size(); i++)
        {
            if (method == methods[i])
                return true;
        }
        return false;
    }


    std::vector<Location *> locations = servers[client.getServerID()]->getLocation();
    for(size_t i = 0; i < locations.size(); i++)
    {
        if (url == locations[i]->getDir() + "/" || url == locations[i]->getDir())
        {
            std::vector<std::string> methods = locations[i]->getMethod();
            for(size_t i = 0; i < methods.size(); i++)
            {
                if (method == methods[i])
                    return true;
            }
        }
    }
    return false;
}

void WebServ::runMethods(ClientSocket &client, std::string url, Request &request)
{
	locations = getLocation(url, client.getServerID());
	if(locations && !locations->getRedir().empty())
        redirect(client, locations->getRedir());
    else if (request.getMethod() == "GET" && isAllowdMethod(client, "GET", url))
        GET(client, url);
    else if (request.getMethod() == "POST" && isAllowdMethod(client, "POST", url))
        POST(client, url, request);
    else if (request.getMethod() == "DELETE" && isAllowdMethod(client, "DELETE", url))
        DELETE(client, url);
}

int containsCgiBin(const std::string& url)
{
    if (url.find("/cgi") != std::string::npos)
        return 1;
    else
        return 0;
}

std::vector<std::string> split(std::string const &str, char delim);

void WebServ::manageClients()
{
    for(size_t i = 0; i < clients.size(); i++)
    {
        if(FD_ISSET(clients[i].getClientSocket(), &readingSet))
        {   
            ssize_t Reqsize = recv(clients[i].getClientSocket(), clients[i].buffer, 65536, 0);
            if (Reqsize == -1)
                exit(69);
            clients[i].buffer[Reqsize] = '\0';
            clients[i].size = Reqsize ;
			std::string str(clients[i].buffer);
			clients[i].request = str.substr(0, Reqsize);
            Request request((char *)clients[i].request.c_str());
            std::cout << request.getRequest() << std::endl;
            try {
                int bodySize = std::stoi(this->servers[clients[i].getServerID()]->getBody());
                if (bodySize < 0 || request.getBody().length() > (size_t) bodySize)
                {
                    this->clientError(413, clients[i]);
                    continue;
                }
            }
            catch (std::exception &e) {
                this->clientError(500, clients[i]);
                continue;
            }
			if (checkRequest(request, clients[i], request.getBody().length()))
				continue;
            std::string url = request.getPath();
			runMethods(clients[i], url, request);
            eraseClient(clients[i]);
        }
    }
}

void WebServ::GET(ClientSocket &client, std::string url)
{
	struct stat s;
    std::string toSend = fixUrl(url, client.getServerID());
    if (url.size() >= 64)return(sendErrorToClient(414, client));
    if (locations && !(locations->getIndex().empty() && url ==  locations->getDir()))
        return (sendResponse(client, locations->getRoot() + locations->getIndex(), 200));
	
		std::ifstream fd( toSend.c_str(), std::fstream::binary );
		stat(toSend.c_str(), &s);
		if(!fd.is_open())
			sendErrorToClient(404, client);
		else
		{
			if(S_ISDIR(s.st_mode))
			{
				if(url == "/")
					sendResponse(client, toSend + servers[client.getServerID()]->getIndex(), 200);
				else if(servers[client.getServerID()]->getautoindex() == "on" || (locations && locations->getautoindex() == "on"))
					autoIndex(client.getClientSocket(), url, toSend, client);
				else
					sendErrorToClient(404, client);
			}
			else
				sendResponse(client, toSend, 200);
		}
		fd.close(); 
}

void WebServ::POST(ClientSocket &client, std::string url, Request &req)
{
    struct stat s;
    std::string toSend = fixUrl(url, client.getServerID());
    if (url.size() >= 64)
        return(sendErrorToClient(414, client));
    std::ifstream fd( toSend.c_str(), std::fstream::binary );
    stat(toSend.c_str(), &s);
    if (!fd.is_open())
        sendErrorToClient(500, client);
    else
    {
        std::string body = req.getBody();
        if (req.getContentType().find("multipart") != std::string::npos)
        {
            if (body.find("filename=") == std::string::npos)
                return (sendErrorToClient(400, client), fd.close());
            std::string filename = body.substr(body.find("filename=") + 10, body.find("\"", body.find("filename=") + 10) - (body.find("filename=") + 10));
            std::string boundary = "\r\n--" + req.getBoundary() + "--";
            std::string content = body.substr(body.find("\r\n\r\n") + 4, body.length() - (body.find("\r\n\r\n") + 5) - boundary.length());
            if (this->locations && !this->locations->getUploadPath().empty())
            {
                std::ofstream newFile(this->locations->getUploadPath() + filename, std::fstream::binary);
                if (!newFile.is_open())
                    return (sendErrorToClient(500, client), fd.close());
                newFile << content;
                newFile.close();
                return (this->sendToClient(client, "HTTP/1.1 201 Created\r\nContent-Length: 0\r\n\r\n"), fd.close());
            }
            else if (this->servers[client.getServerID()]->getUploadPath().empty())
            {
                std::ofstream newFile(this->servers[client.getServerID()]->getUploadPath() + filename, std::fstream::binary);
                if (!newFile.is_open())
                    return (sendErrorToClient(500, client), fd.close());
                newFile << content;
                newFile.close();
                return (this->sendToClient(client, "HTTP/1.1 201 Created\r\nContent-Length: 0\r\n\r\n"), fd.close());
            }
            return (this->sendToClient(client, "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n"), fd.close());
        }
        else if (req.getContentType().find("urlencoded") != std::string::npos)
        {
            std::string filename = body.substr(body.find("=") + 1);
            std::ifstream file(filename.c_str(), std::fstream::binary);
            if (!file.is_open())
                return (sendErrorToClient(500, client), fd.close());
            else
            {
                std::string line, path;

                if (this->locations && !this->locations->getUploadPath().empty())
                    path = this->locations->getUploadPath();
                else if (!this->servers[client.getServerID()]->getUploadPath().empty())
                    path = this->servers[client.getServerID()]->getUploadPath();
                else
                    return (this->sendToClient(client, "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n"), fd.close());

                std::ofstream newFile(path + filename, std::fstream::binary);
                if (!newFile.is_open())
                    return (sendErrorToClient(500, client), fd.close());
                const int bufferSize = 2048;
                char buffer[bufferSize];
                while (file.read(buffer, bufferSize))
                {
                    newFile.write(buffer, bufferSize);
                    memset(buffer, 0, bufferSize);
                }
                std::streamsize bytesRead = file.gcount(); // last chunk
                if (bytesRead > 0)
                    newFile.write(buffer, bytesRead);
                file.close();
                newFile.close();
                return (this->sendToClient(client, "HTTP/1.1 201 Created\r\nContent-Length: 0\r\n\r\n"), fd.close());
            }
        }
        else
        {
            std::string filename = this->getRandomName();
            std::string path;
            if (this->locations && !this->locations->getUploadPath().empty())
                path = this->locations->getUploadPath();
            else if (!this->servers[client.getServerID()]->getUploadPath().empty())
                path = this->servers[client.getServerID()]->getUploadPath();
            else
                return (this->sendToClient(client, "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n"), fd.close());
            std::ofstream newFile(path + filename, std::fstream::binary);
            if (!newFile.is_open())
                return (sendErrorToClient(500, client), fd.close());
            newFile << body;
            newFile.close();
            return (this->sendToClient(client, "HTTP/1.1 201 Created\r\nContent-Length: 0\r\n\r\n"), fd.close());
        }
    }
    fd.close();
}

std::string WebServ::getRandomName(void)
{
    std::string name = "tmp";
    std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < 10; i++)
        name += charset[rand() % charset.length()];
    return (name);
}

void removeFile(std::string path)
{
    struct stat st;
    struct dirent *direct;
    std::string deleted ;
    stat(path.c_str(), &st);
    DIR *dir = opendir(path.c_str());
    while (S_ISDIR(st.st_mode) && (direct = readdir(dir)) != NULL)
    {
        deleted = path + direct->d_name;
        struct stat sta;
        stat(deleted.c_str(), &sta);
        if (!strcmp(direct->d_name, ".") || !strcmp(direct->d_name, "..") || !strcmp(direct->d_name, "/"));
        else
        {
            if (S_ISDIR(sta.st_mode))
                removeFile(path + direct->d_name);
            else if (!S_ISDIR(sta.st_mode))
                unlink(((const char *)deleted.c_str()));
        }
    }
    if (dir)
        closedir(dir);
    remove(path.c_str());
}


std::string findPWD(char **envp)
{
    const char* value = nullptr;
    const char* key = "PWD=";

    
    for (int i = 0; envp[i] != nullptr; ++i) {
        if (strncmp(envp[i], key, strlen(key)) == 0) {
            value = envp[i] + strlen(key);
            break;
        }
    }
    std::string ret(value);
    return ret;
}


void WebServ::DELETE(ClientSocket &client, std::string url)
{
    std::string root = getLocationRoot(url, client);
    std::string path = findPWD(envp) + root;
	struct stat s;
    stat(path.c_str(), &s);
    if (!S_ISDIR(s.st_mode))
        unlink(((const char *)path.c_str()));
    else
        removeFile(path);
    std::string response = (char *)"HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Length: 70";
    response += "\r\n\r\n<!DOCTYPE html><head><title>200 OK</title></head><body> </body></html>";
    send(client.getClientSocket(), response.c_str(), response.size(), 0);
}

void pexit(std::string err)
{
	perror(err.c_str());
	exit(1);
}

void WebServ::parseMimeTypes(void)
{
	std::ifstream file("mime.types");

	if (file.is_open())
	{
		std::string line;
		while (std::getline(file, line))
		{
			line = trimString(line);
			if (line.empty())
				continue;

			std::istringstream iss(line);
			std::string mimeType;
			iss >> mimeType;
			if (mimeType == "types" || mimeType == "}")
				continue;

			std::string extension;
			while (iss >> extension)
			{
				if (extension[extension.size() - 1] == ';')
					extension = extension.substr(0, extension.size() - 1);
				this->_mimeTypes[extension] = mimeType;
			}
		}
		file.close();
	}
	else
	{
		std::cerr << "Error: mime.types.conf file not found" << std::endl;
		exit(1);
	}
}

void WebServ::printMimeTypes(void)
{
	for (std::map<std::string, std::string>::iterator it = this->_mimeTypes.begin(); it != this->_mimeTypes.end(); it++)
		std::cout << it->first << " " << it->second << std::endl;
}

std::string WebServ::getMimeType(std::string &extension)
{
	try
	{
		return this->_mimeTypes.at(extension);
	}
	catch (const std::exception &e)
	{
		return "text/plain";
	}
}

std::string WebServ::getExtensionFromUrl(const std::string& url)
{
    std::size_t lastDotPos = url.find_last_of('.');
    if (lastDotPos == std::string::npos || lastDotPos == url.length() - 1)
        return ""; 
    else
        return url.substr(lastDotPos + 1);
}

std::string WebServ::getMimeTypeFromExtension(const std::string& url)
{
    std::string extension = getExtensionFromUrl(url);
    if (!extension.empty())
	{
        std::map<std::string, std::string>::const_iterator it = _mimeTypes.find(extension);
        if (it != _mimeTypes.end())
		{
            return it->second;
        }
    }
    return "";
}

std::string trimString(const std::string& str) {
    std::string trimmed = str;
    std::string::size_type pos = 0;
    while (pos < trimmed.length() && (trimmed[pos] == ' ' ||  trimmed[pos] == '\t')) {
        ++pos;
    }
    trimmed.erase(0, pos);
    pos = trimmed.length() - 1;
    while (pos > 0 && (trimmed[pos] == ' ' || trimmed[pos] == '\t')) {
        --pos;
    }
    trimmed.erase(pos + 1);
    return trimmed;
}

void WebServ::run()
{
    while(1337)
    {
        clientsQueue();
        createClients();
        manageClients();
    }
}
