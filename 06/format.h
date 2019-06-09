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

struct streamable_any : std::any {
    void(*streamer)(std::ostream&, streamable_any const&) = nullptr;
    friend std::ostream& operator<<( std::ostream& os, streamable_any const& a )
    {
        a.streamer( os, a );
        return os;
    }
    
    template<class T, std::enable_if_t<!std::is_same<std::decay_t<T>, streamable_any>{}, bool> = true>
    streamable_any( T&& t ):
    std::any (std::forward<T>(t) ), streamer([](std::ostream& os, streamable_any const& self ){os << std::any_cast<std::decay_t<T>>(self);})
    {}
};

std::vector<streamable_any> vargs;

template <typename... ArgTypes> void to_vector(ArgTypes... Args);
template <typename T, typename... ArgTypes> void to_vector(T t, ArgTypes... Args)
{
    vargs.push_back(t);
    to_vector(Args...);
}
template <> void to_vector() {}

template <typename... ArgTypes> std::string format(std::string s, ArgTypes... Args)
{
    char c;
    std::ostringstream res;
    vargs.clear();
    to_vector(Args...);
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
                throw std::runtime_error("a");
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
                throw std::runtime_error("b");
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


#endif
