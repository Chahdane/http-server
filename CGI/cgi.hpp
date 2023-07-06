#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include "../Parsing/Server.hpp"
#include "../Parsing/Request.hpp"
#include "../Parsing/Location.hpp"

class CGI {

    public:
        CGI(const Request& request);
        void   setInputPath(const std:: string& path);
        void   setOutputPath(const std:: string& path);
        void   run_cgi();
        size_t getLength();

    private:
        std::string         InputPath;
        std::string         OutputPath;
        size_t              contentLength;
        const Request&      _request;
        // Location&    loc;
        std::string  scriptPath;

        void   Handle_exec();
        void   SetIO();
        const  std::string& Find_cgi();
        void   EnvVariables();
        void   setLenght();
        // std::map<std::string, std::string> cgiExecutables;
};

#endif