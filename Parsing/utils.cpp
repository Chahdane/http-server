#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

std::string trim(const std::string &str)
{
    std::string trimmedStr = str;
    std::string::iterator start = trimmedStr.begin();
    while (start != trimmedStr.end() && std::isspace(*start))
        ++start;
    std::string::iterator end = trimmedStr.end();
    while (end != start && std::isspace(*(end - 1)))
        --end;
    trimmedStr.erase(end, trimmedStr.end());
    trimmedStr.erase(trimmedStr.begin(), start);
    std::string::iterator it = trimmedStr.begin();
    while (it != trimmedStr.end())
    {
        if (std::isspace(*it))
        {
            std::string::iterator next = it + 1;
            if (next != trimmedStr.end() && std::isspace(*next))
                trimmedStr.erase(next);
            else
                ++it;
        }
        else
            ++it;
    }
    return trimmedStr;
}

std::vector<std::string> split(std::string const &str, char delim)
{
    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    std::string::size_type end = str.find(delim, start);
    while (end != std::string::npos)
    {
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delim, start);
    }
    tokens.push_back(str.substr(start));
    return tokens;
}
