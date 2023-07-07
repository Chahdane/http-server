#pragma once
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

std::vector<std::string> split(std::string const &str, char delim);
std::string trim(const std::string &str);

class Request
{
	private:
        std::string _request;
        std::string _method;
        std::string _path;
        std::string _queries;
        std::string _protocol;
        std::string _host;
        std::string _encoding;
        std::string _contentType;
        std::string _boundary;
        unsigned int _contentLength;
        std::string _body;
        std::map<std::string, std::string> _cookie;
	
    public:
		std::string			getRequest() {return _request;};
        std::string			getMethod() {return _method;};
        std::string			getPath() {return _path;};
		std::string			getQueries() {return _queries;};
        std::string			getProtocol() {return _protocol;};
		std::string			getEncoding() {return _encoding;}
		std::string			getContentType() {return _contentType;}
        std::string			getBoundary() const	{return _boundary;};
        size_t				getContentLength() const		{return _contentLength;};
        std::string			getBody() const	{return _body;};
        std::map<std::string, std::string>	getCookie() const	{return _cookie;};

        void                setBody(std::string body) {_body = body;};
        

        Request(std::string req)
            : _request(req), _method(""), _path(""), _queries(""), _protocol(""), _host(""), _encoding(""), _contentType(""), _boundary(""), _contentLength(0), _body(""), _cookie()
            {
                std::stringstream ss(req);
                _request = req;
                _contentLength = 0;
                ss >> this->_method;
                ss >> this->_path;
                ss >> this->_protocol;
                setRequest(req);
                parseRequest();
            }
        Request(const Request &copy)
        {
            *this = copy;
        }
        ~Request() {}

        void setRequest(const std::string &request)
        {
            this->_request = request;
        }

        bool parseSingleLinedAttributes(std::string &line)
        {
            std::string __cookie_str = "";
            std::string const attributes[6] = {"Host: ", "Accept-Encoding: ", "Content-Type: ", "boundary=", "Cookie: ", "Content-Length: "};
            std::string *attribute[5] = {&this->_host, &this->_encoding, &this->_contentType, &this->_boundary, &__cookie_str};
            bool isFound = false;
            for (int i = 0; i < 5; i++)
                if (line.find(attributes[i]) != std::string::npos)
                    *attribute[i] = line.substr(line.find(attributes[i]) + attributes[i].length()), isFound = true;
            if (line.find(attributes[5]) != std::string::npos)
                this->_contentLength = std::stoi(line.substr(line.find(attributes[4]) + attributes[4].length())), isFound = true;
            if (__cookie_str.length() > 0)
            {
                std::vector<std::string> cookies = split(__cookie_str, ';');
                for (std::vector<std::string>::iterator it = cookies.begin(); it != cookies.end(); it++)
                {
                    std::vector<std::string> cookie = split(*it, '=');
                    if (cookie.size() == 2)
                        this->_cookie[trim(cookie[0])] = trim(cookie[1]);
                }
            }
            return isFound;
        }

        void parseRequest(void)
        {
            std::string line;
            std::istringstream iss(this->_request);
            while (std::getline(iss, line))
            {
                if (this->parseSingleLinedAttributes(line))
                    continue;
                else if (line == "\r")
                    break;
            }
            if (this->_contentLength > 0)
            {
                while (std::getline(iss, line, '\0'))
                {
                    this->_body = line;
                    if (this->_body.length() >= this->_contentLength)
                    {
                        this->_body.resize(this->_contentLength);
                        break;
                    }
                }
            }
            if (this->_contentType.length() > 0 && this->_contentType.find(";") != std::string::npos)
                this->_contentType = this->_contentType.substr(0, this->_contentType.find(";"));
        }

        void printRequest(void)
        {
        	std::cout << "Method: " << this->_method << std::endl;
        	std::cout << "Path: " << this->_path << std::endl;
        	std::cout << "Queries: " << this->_queries << std::endl;
        	std::cout << "Protocol: " << this->_protocol << std::endl;
        	std::cout << "Host: " << this->_host << std::endl;
        	std::cout << "Encoding: " << this->_encoding << std::endl;
        	std::cout << "Content-Type: " << this->_contentType << std::endl;
        	std::cout << "Boundary: " << this->_boundary << std::endl;
        	std::cout << "Content-Length: " << this->_contentLength;
        	if (this->_contentLength > 0 && this->_body.length() > 0)
        		std::cout << std::endl
        				  << std::endl
        				  << this->_body << std::endl;
        }

        // void print_req()
		// {

		// 	std::cout << "_method :" << _method << std::endl;
		// 	std::cout << "_path :" << _path << std::endl;
		// 	std::cout << "_protocol :" << _protocol << std::endl;
		// 	std::cout << "_boundary :" << _boundary << std::endl;
		// 	std::cout << "_len :" << _contentLength << std::endl;
		// 	std::cout << "_request :" << _request << std::endl;
		// 	std::cout << "_query :" << _queries << std::endl;
        //     std::cout << "type :" << _contentLength << std::endl;
        //     std::cout << "encoding :" << _encoding << std::endl;
        //     std::cout << "body :" << _body << std::endl;
		// }
};
