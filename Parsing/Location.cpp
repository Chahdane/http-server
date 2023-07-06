#include "Parsing.hpp"

Location::Location()
    : _dir(""), _root(""), method(), _redir(""), cgi(), indexs(), _upload_path("")
{
}

Location::Location(const Location &copy)
{
    *this = copy;
}

Location &Location::operator=(const Location &copy)
{
    if (this != &copy)
    {
        this->_dir = copy._dir;
        this->_root = copy._root;
        this->method = copy.method;
        this->_redir = copy._redir;
        this->indexs = copy.indexs;
        this->cgi = copy.cgi;
        this->_upload_path = copy._upload_path;
    }
    return (*this);
}

Location::~Location()
{
}

void Location::setPath(std::string path)
{
    this->_dir = path;
}
void Location::setRoot(std::string root)
{
    this->_root = root;
}
void Location::setAllowedMethods(std::vector<std::string> allowed_methods)
{
    this->method = allowed_methods;
}
void Location::setRedirection(std::string redirection)
{
    this->_redir = redirection;
}
void Location::setCgi(std::map<std::string, std::string> cgi)
{
    this->cgi = cgi;
}
void Location::setIndexs(std::vector<std::string> indexs)
{
    this->indexs = indexs;
}
void Location::setUploadPath(std::string upload_path)
{
    this->_upload_path = upload_path;
}

void Location::addCgi(std::string extension, std::string cgi)
{
    this->cgi.insert(std::pair<std::string, std::string>(extension, cgi));
}

std::string Location::getDir() const
{
    return (this->_dir);
}
std::string Location::getRoot() const
{
    return (this->_root);
}
std::vector<std::string> Location::getMethod() const
{
    return (this->method);
}
std::string Location::getRedir() const
{
    return (this->_redir);
}
std::map<std::string, std::string> const &Location::getCgi() const
{
    return (this->cgi);
}
std::vector<std::string> Location::getIndexs() const
{
    return (this->indexs);
}
std::string Location::getUploadPath() const
{
    return (this->_upload_path);
}
