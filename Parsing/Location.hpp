#pragma once

#include <iostream>
#include <vector>

class Location
{
public:
	Location(void);
	Location(const Location &copy);
	Location &operator=(const Location &copy);
	~Location(void);

	void setPath(std::string path);
	void setRoot(std::string root);
	void setAllowedMethods(std::vector<std::string> allowed_methods);
	void setRedirection(std::string redirection);
    void setCgi(std::map<std::string, std::string> cgi);
	void setIndexs(std::vector<std::string> indexs);
	void setIndex(std::string word)		{_index = word;};
	void setautoindex(std::string word)	{_autoindex = word;};
    void setUploadPath(std::string upload_path);

    void addCgi(std::string extension, std::string cgi);

	std::string getDir(void) const;
	std::string getRoot(void) const;
	std::vector<std::string> getMethod(void) const;
	std::string getRedir(void) const;
    std::map<std::string, std::string> const &getCgi(void) const;
	std::vector<std::string> getIndexs(void) const;
	std::string					getIndex()		{return _index;};
	std::string					getautoindex()	{return _autoindex;};
    std::string                 getUploadPath(void) const;

private:
	std::string _dir;
	std::string _root;
	std::vector<std::string> method;
	std::string _redir;
    std::map<std::string, std::string> cgi;
	std::vector<std::string> indexs;
	std::string					_index;
	std::string					_autoindex; //
    std::string                 _upload_path;

};
