#include "./WebServer/WebServer.hpp"
#include "./Parsing/Parsing.hpp"

int main(int ac, char **av, char **envp)
{
    if (ac != 2)  return (1);
    Conf config_file;
    config_file.setFileName(av[1]);
    config_file.parse();
	WebServ serv(envp, config_file.getServers());
    serv.run();
    // free fds;
}
