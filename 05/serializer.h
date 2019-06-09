#ifndef serializer_h
#define serializer_h

#include <iostream>
#include <string>
#include <sstream>
#include <exception>

enum class Error
{
    NoError,
    CorruptedArchive
};

class Serializer
{
public:
    std::ostream& out_;
    explicit Serializer(std::ostream& out) : out_(out) {}
    
    template <class T>
    Error save(T& object)
    {
        return object.serialize(*this);
    }
    
    template <class... ArgsT>
    Error operator()(ArgsT... args)
    {
        return process(args...);
    }
    
    template <class... Args>
    Error process(Args... args);
    template <class T, class... Args>
    Error process(T t, Args... args)
    {
        if (process(t) == Error::NoError)
        {
            return process(args...);
        }
        return Error::CorruptedArchive;
    }
    Error process(bool t)
    {
        if (t)
        {
            out_ << "true ";
        }
        else
        {
            out_ << "false ";
        }
        return Error::NoError;
    }
    Error process(uint64_t t)
    {
        out_ << t << " ";
        return Error::NoError;
    }
    Error process()
    {
        return Error::CorruptedArchive;
    }
};

class Deserializer
{
public:
    std::istream& in_;
    explicit Deserializer(std::istream& in) : in_(in) {}
    template <class T>
    Error load(T& object)
    {
        return object.serialize(*this);
    }
        
    template <class... Args>
    Error operator()(Args&... args)
    {
        return process(args...);
    }
    
    template <class... Args>
    Error process(Args&... args);
    template <class T, class... Args>
    Error process(T& t, Args&... args)
    {
        if (process(t) == Error::NoError)
        {
            return process(args...);
        }
        return Error::CorruptedArchive;
    }
    Error process(bool& t)
    {
        std::string text;
        if (in_ >> text)
        {
            if (text == "true")
            {
                t = true;
            }
            else if (text == "false")
            {
                t = false;
            }
            else
            {
                return Error::CorruptedArchive;
            }
            return Error::NoError;
        }
        return Error::CorruptedArchive;
    }
    Error process(uint64_t& t)
    {
        std::string text;
        if (in_ >> text)
        {
            if (text[0] == '-')
            {
                return Error::CorruptedArchive;
            }
            try
            {
                t = stoul(text);
            }
            catch(std::exception &e)
            {
                return Error::CorruptedArchive;
            }
            return Error::NoError;
        }
        return Error::CorruptedArchive;
    }
    Error process()
    {
        return Error::CorruptedArchive;
    }
};
    
#endif
