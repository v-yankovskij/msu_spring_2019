#ifndef format_h
#define format_h

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <memory>
#include <any>

template <typename... ArgTypes> void to_vector(std::vector<std::string>& vargs, ArgTypes... Args);
template <typename T, typename... ArgTypes> void to_vector(std::vector<std::string>& vargs, T t, ArgTypes... Args)
{
    std::ostringstream os;
    os << t;
    vargs.push_back(os.str());
    to_vector(vargs, Args...);
}
template <> void to_vector(std::vector<std::string>& vargs) {}

template <typename... ArgTypes> std::string format_(std::string s, ArgTypes... Args)
{
    char c;
    std::ostringstream res;
    std::vector<std::string> vargs;
    vargs.clear();
    to_vector(vargs, Args...);
    for (int i = 0; i < s.length(); i++)
    {
        if (s[i] == '{')
        {
            i++;
            int num = 0;
            while ((i < s.length()) && (s[i] >= '0') && (s[i] <= '9'))
            {
                num *= 10;
                num += (s[i] - '0');
                i++;
            }
            if ((i == s.length())||(s[i] != '}')||(num >= vargs.size()))
            {
                vargs.clear();
                throw std::runtime_error("");
            }
            else
            {
                res << vargs[num];
            }
        }
        else
        {
            if (s[i] == '}')
            {
                throw std::runtime_error("");
            }
            else
            {
                res << s[i];
            }
        }
    }
    vargs.clear();
    return res.str();
}


template <typename... ArgTypes> std::string format(std::string&& s, ArgTypes&&... Args)
{
    return format_(std::forward<std::string>(s), std::forward<ArgTypes>(Args)...);
}


#endif
