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
	void setIndexs(std::vector<std::string> indexs);
	void setIndex(std::string word)		{_index = word;};
	void setautoindex(std::string word)	{_autoindex = word;};

	std::string getDir(void) const;
	std::string getRoot(void) const;
	std::vector<std::string> getMethod(void) const;
	std::string getRedir(void) const;
	std::vector<std::string> getIndexs(void) const;
	std::string					getIndex()		{return _index;};
	std::string					getautoindex()	{return _autoindex;};

private:
	std::string _dir;
	std::string _root;
	std::vector<std::string> method;
	std::string _redir;
	std::vector<std::string> indexs;
	std::string					_index;
	std::string					_autoindex; //

};



// class Location
// {
// 	public:
// 		Location() {;};
// 		~Location() {_method.clear();};
		
// 		/* Setteres */
// 		void setDir(std::string word)		{_dir = word;};
// 		void setRoot(std::string word)		{_root = word;};
// 		void setIndex(std::string word)		{_index = word;};
// 		void setMethod(std::string word)	{_method.push_back(word);};
// 		void setautoindex(std::string word)	{_autoindex = word;};
// 		void setRedir(std::string word)		{_redir = word;};

// 		/* Getters */
// 		std::string					getDir()		{return _dir;};
// 		std::vector<std::string>	getMethod()		{return _method;};
// 		std::string					getRoot()		{return _root;};
// 		std::string					getIndex()		{return _index;};
// 		std::string					getautoindex()	{return _autoindex;};
// 		std::string					getRedir()		{return _redir;};
		
// 	private:
// 		std::string					_dir;
// 		std::vector<std::string>	_method;
// 		std::string					_root;
// 		std::string					_index;
// 		std::string					_autoindex;
// 		std::string					_redir;
// };
