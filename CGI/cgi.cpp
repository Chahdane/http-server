#include "cgi.hpp"
#include <sys/stat.h>
#include "../Parsing/Server.hpp"
#include "../Parsing/Location.hpp"

CGI::CGI(const Request& request, Location *location)
    : contentLength()
    , _request(request)
    {
        // Location loc;
        // loc.setRoot("/CGI/CGI-bin/test.php");
         scriptPath = location->getRoot();
     std::cout << "scriptPath"<< scriptPath <<"whyyyyy" <<"\n";
    }

unsigned int getFileSize(const std::string& path) {
	struct stat pathInfo;
	if (stat(path.c_str(), &pathInfo)) {
		throw std::runtime_error("couldn't get size of file");
	}
	return (pathInfo.st_size);
}

void    CGI::run_cgi()
{
    Handle_exec();
    //if statuscode < 400 -->> setlength of content ()
}


size_t CGI::getLength() {
	return contentLength;
}

void    CGI::Handle_exec()
{
    const pid_t pid = fork();
    int exec;

    if (pid == 0)
    {
        EnvVariables();
        SetIO();
        const std::string& executable = Find_cgi();
        exec = execl(executable.c_str(), executable.c_str(), scriptPath.c_str(), NULL);
        if (exec == -1)
			exit(EXIT_FAILURE);
        sleep(1);
        pid_t value = waitpid(pid, NULL, WNOHANG);
        if(value < 1)
        {
            if(value == 0)
            {
                kill(pid, SIGKILL);
                throw std::runtime_error("time out");
            }
            return ;
        }
    }

}

void    CGI::EnvVariables()
{
    const bool overwrite = true;

    setenv("PATH_INFO", scriptPath.c_str(), overwrite);
    setenv("SCRIPT_FILENAME", scriptPath.c_str(), overwrite);
    setenv("SCRIPT_NAME", scriptPath.c_str(), overwrite);
    setenv("REDIRECT_STATUS", "200", overwrite);
    const std::string& querystr = _request.getQueries();
	setenv("QUERY_STRING", querystr.c_str(), overwrite);
    // const std::string cookie = _request.getCookie();
	//  if (cookie)
	// 	    setenv("HTTP_COOKIE", cookie.c_str(), overwrite);
    const std::string method = _request.getMethod();
    std::cout<<"test" << method<<"\n";
    if (method == "GET")
        setenv("REQUEST_METHOD", "GET", overwrite);
    else if (method == "POST")
    {
        setenv("REQUEST_METHOD", "POST", overwrite);
        size_t contentLenght = _request.getContentLength();
        if (contentLenght > 0)
            setenv("CONTENT_LENGTH", std::to_string(contentLenght).c_str(), overwrite);
        // else
        // {
        //     const std::string transfer_encoding = _request.getTransferEncoding();
        //     if (!transfer_encoding.empty() && (transfer_encoding == "chunked"))
        //     {
        //         unsigned int chunkedSize = getFileSize(_request.getPathToBodyFileName());
		// 		setenv("CONTENT_LENGTH", std::to_string(chunkedSize).c_str(), overwrite);
        //     }
        //     else
        //         throw std::runtime_error("error cgi");
        // }
        const std::string contntType = _request.getContentType();
        if (!contntType.empty())
            throw std::runtime_error("error cgi");
        setenv("CONTENT_TYPE", contntType.c_str(), overwrite);
    }
    else
        throw std::runtime_error("error cgi");
}

void    CGI::SetIO()
{
    if (_request.getMethod() == "POST")
    {
        int input = open(InputPath.c_str(), O_RDONLY);
        if (input == -1)
            throw std::runtime_error("couldn't open");
        if (dup2(input, 0) == -1) // 0 as in STDIN_FILENO
            throw std::runtime_error("couldn't dup");
        int output = open(OutputPath.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0666);
        if (output == -1)
            throw std::runtime_error("couldn't open");
        if(dup2(output, 0) == -1)
            throw std::runtime_error("couldn't dup");
    }   
}

const std::string& CGI::Find_cgi()
{
    Server server;
    const std::string::size_type dotposition = scriptPath.rfind('.');
    const std::string type = scriptPath.substr(dotposition + 1);
    std::map<std::string, std::string> cgiExecutables;
    cgiExecutables.insert(std::pair<std::string, std::string>("php", "/test"));
    // cgiExecutables = server.getCgi();
    std::map<std::string, std::string>::const_iterator it = cgiExecutables.find(type);

    if (it != cgiExecutables.end())
        return it->second;
    else
        throw std::runtime_error("No executable found for type: " + type);
}

void CGI::setInputPath
	(const std::string& path) {
	InputPath = path;
}

void CGI::setOutputPath
	(const std::string& path) {
	OutputPath = path;
}