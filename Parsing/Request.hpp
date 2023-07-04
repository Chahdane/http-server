#pragma once

# include <iostream>
# include <sstream>
# include <string>

class Request
{
	private:
		std::string		_request;
		std::string		_method;
		std::string		_path;
		std::string		_queries;
		std::string		_protocol;
		std::string		_boundary;
		size_t			_contentLength;
		std::string		_body;

	public:
		Request(char *Request)
		{
			std::stringstream input(Request);
			_request = Request;
			_contentLength = 0;	
			input >> _method;
			input >> _path;
			input >> _protocol;
		};
		std::string		getRequest() {return _request;};
		std::string		getMethod() {return _method;};
		std::string		getPath() {return _path;};
		std::string		getProtocol() {return _protocol;};
		std::string		getBoundary() {return _boundary;};
		size_t			getContentLength() {return _contentLength;};
		std::string		getBody() {return _body;};
		std::string		getQueries() {return _queries;};
		
		void print_req()
		{
			std::cout << "_method :" << _method << std::endl;
			std::cout << "_path :" << _path << std::endl;
			std::cout << "_protocol :" << _protocol << std::endl;
			std::cout << "_boundary :" << _boundary << std::endl;
			std::cout << "_len :" << _contentLength << std::endl;
			std::cout << "_request :" << _request << std::endl;
			std::cout << "_query :" << _queries << std::endl;
            std::cout << "type :" << _contentLength << std::endl;
            std::cout << "body :" << _body << std::endl;
		}

};

