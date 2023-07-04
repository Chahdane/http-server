#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include "Server.hpp"

# define RED	"\x1B[31m"
# define RESET	"\x1B[0m"

class Conf
{
public:
	Conf(void);
	Conf(const Conf &copy);
	Conf &operator=(const Conf &copy);
	~Conf(void);

	void setFileName(std::string file_name);
	void setServers(std::vector<Server *> servers);

	std::string getFileName(void) const;
	std::vector<Server *> getServers(void) const;

	void print(void) const;
	void print_all_data();

	void parse(void);
	void parseServerLine(std::vector<std::string> tokens, Server *server);
	void parseLocationLine(std::vector<std::string> tokens, Location *location);

private:
	std::string file_name;
	std::vector<Server *> _servers;
};

