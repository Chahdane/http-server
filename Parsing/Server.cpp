#include "Server.hpp"

Server::Server()
    : _name(""), ip(""), _listen("80"), _method(), _error_pages(), _locations(), cgi(), _root(""), _index(""), _body_size("0"), _autoindex("off"), _upload_path("")
{
}

Server::Server(const Server &copy)
{
    *this = copy;
}

Server &Server::operator=(const Server &copy)
{
    if (this != &copy)
    {
        this->_name = copy._name;
        this->ip = copy.ip;
        this->_listen = copy._listen;
        this->_method = copy._method;
        this->_error_pages = copy._error_pages;
        this->_locations = copy._locations;
        this->cgi = copy.cgi;
        this->_root = copy._root;
        this->_index = copy._index;
        this->_body_size = copy._body_size;
        this->_upload_path = copy._upload_path;
        this->_autoindex = copy._autoindex;
    }
    return (*this);
}

Server::~Server()
{
}

void Server::setServerName(std::string _name)
{
    this->_name = _name;
}
void Server::setIp(std::string ip)
{
    this->ip = ip;
}
void Server::setPort(std::string _listen)
{
    this->_listen = _listen;
}
void Server::setMethods(std::vector<std::string> methods)
{
    this->_method = methods;
}
void Server::setErrorPages(std::map<std::string, std::string> _error_pages)
{
    this->_error_pages = _error_pages;
}
void Server::setLocations(std::vector<Location *> _locations)
{
    this->_locations = _locations;
}
void Server::setCgi(std::map<std::string, std::string> cgi)
{
    this->cgi = cgi;
}
void Server::setRoot(std::string _root)
{
    this->_root = _root;
}
void Server::setIndex(std::string index)
{
    this->_index = index;
}
void Server::setMaxClientBodySize(std::string _body_size)
{
    this->_body_size = _body_size;
}
void Server::setUploadPath(std::string upload_path)
{
    this->_upload_path = upload_path;
}

void Server::addMethod(std::string method)
{
    this->_method.push_back(method);
}
void Server::addErrorPage(std::string error_code, std::string error_page)
{
    this->_error_pages.insert(std::pair<std::string, std::string>(error_code, error_page));
}
void Server::addLocation(Location *location)
{
    this->_locations.push_back(location);
}
void Server::addCgi(std::string extension, std::string cgi)
{
    this->cgi.insert(std::pair<std::string, std::string>(extension, cgi));
}

std::string const &Server::getName() const
{
    return (this->_name);
}   
std::string const &Server::getIp() const
{
    return (this->ip);
}
std::string const &Server::getListen() const
{
    return (this->_listen);
}
std::vector<std::string> const &Server::getMethod() const
{
    return (this->_method);
}
std::map<std::string, std::string> const &Server::getError() const
{
    return (this->_error_pages);
}
std::vector<Location *> const &Server::getLocation() const
{
    return (this->_locations);
}
std::map<std::string, std::string> const &Server::getCgi() const
{
    return (this->cgi);
}
std::string const &Server::getRoot() const
{
    return (this->_root);
}
std::string const &Server::getIndex() const
{
    return (this->_index);
}
std::string const &Server::getBody() const
{
    return (this->_body_size);
}
std::string const &Server::getUploadPath() const
{
    return (this->_upload_path);
}
