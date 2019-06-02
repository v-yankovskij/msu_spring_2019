#ifndef bigint_h
#define bigint_h

#include <iostream>
#include "stdio.h"

class BigInt
{
public:
    int64_t res;
    bool nonn;
    BigInt* quot;
    
    BigInt(int64_t b)
    {
        int64_t a = b;
        if (a < 0)
        {
            a = -a;
            nonn = false;
        }
        else nonn = true;
        res = (a%100000);
        if (a/100000 == 0)
        {
            quot = nullptr;
        }
        else
        {
            quot = new BigInt(a/100000);
        }
    }

    BigInt()
    {
        nonn = true;
        res = 0;
        quot = nullptr;
    }
    
    void brief()
    {
        if (quot != nullptr)
        {
            quot->nonn = true;
            if (quot->quot == nullptr)
            {
                if (quot->res == 0)
                {
                    quot = nullptr;
                }
            }
            else
            {
                quot->brief();
            }
        }
    }
    
    BigInt& operator=(const BigInt& copied)
    {
        nonn = copied.nonn;
        res = copied.res;
        if (copied.quot != nullptr)
        {
            if (quot == nullptr)
            {
                quot = new BigInt(*(copied.quot));
            }
            else *quot = BigInt(*(copied.quot));
        }
        else quot = nullptr;
        return *this;
    }

    BigInt& operator=(const int64_t& copied)
    {
        BigInt temp = BigInt(copied);
        *this = temp;
        return *this;
    }
    
    BigInt(const BigInt& copied)
    {
        res = copied.res;
        nonn = copied.nonn;
        if (copied.quot != nullptr)
        {
            quot = new BigInt(*copied.quot);
        }
        else quot = nullptr;
    }
    
    BigInt operator- () const
    {
        BigInt tmp(*this);
        tmp.nonn = !nonn;
        return tmp;
    }
    
    BigInt operator+ (const BigInt& other) const
    {
        BigInt a = BigInt(*this);
        BigInt b = BigInt(other);
        int64_t temp;
        if (a.nonn && b.nonn)
        {
            temp = a.res + b.res;
        }
        if (!a.nonn && b.nonn)
        {
            temp = -a.res + b.res;
        }
        if (a.nonn && !b.nonn)
        {
            temp = a.res - b.res;
        }
        if (!a.nonn && !b.nonn)
        {
            temp = -a.res - b.res;
        }
        if ((a.quot == nullptr)&&(b.quot != nullptr))
        {
            a.quot = new BigInt(0);
        }
        if ((b.quot == nullptr)&&(a.quot != nullptr))
        {
            b.quot = new BigInt(0);
        }
        if ((a.quot != nullptr)&&(b.quot != nullptr))
        {
            (a.quot)->nonn = a.nonn;
            (b.quot)->nonn = b.nonn;
            *(a.quot) = *(a.quot) + *(b.quot);
        } 
        if ((a.quot != nullptr)&&(b.quot != nullptr))
        {
            if (temp >= 0)
            {
                if ((a.quot)->nonn)
                {
                    a.res = temp%100000;
                }
                else
                {
                    a.res = 100000-temp%100000;
                    (a.quot)->res++;
                }
            }
            else
            {
                if ((a.quot)->nonn)
                {
                    a.res = 100000+temp%100000;
                    (a.quot)->res--;
                }
                else
                {
                    a.res = -temp%100000;
                }
            }
            temp /= 100000;            
            a.nonn = (a.quot)->nonn;
            (a.quot)->nonn = true;
        }
        else
        {
            a.nonn = (temp >= 0);
            if (temp >= 0)
            {
                a.res = temp%100000;
            }
            else
            {
                a.res = -temp%100000;  
            }    
            temp /= 100000;
        }
        if (temp != 0)
        {
            if ((a.quot == nullptr))
            {
                if (temp > 0)
                {
                    a.quot = new BigInt(temp);
                }
                else
                {
                    a.quot = new BigInt(-temp);
                    a.nonn = !a.nonn;
                }
            }
            else
            {
                if (temp > -((a.quot)->res))
                {
                    (a.quot)->res += temp;
                }
                else 
                {
                    (a.quot)->res += temp;
                    if ((a.quot)->quot == nullptr)
                    {
                        (a.quot)->res = -((a.quot)->res);
                        a.nonn = !a.nonn;
                    }
                    else
                    {
                        (a.quot)->res += 100000;
                    }
                }
            }
        }
        a.brief();
        return a;
    }
    
    BigInt operator- (const BigInt& other) const
    {
        return *this + -(other);
    }
    
    BigInt operator+=(const BigInt& other)
    {
        BigInt tmp(*this);
        tmp = tmp + other;
        return tmp;
    }
    
    BigInt operator-=(const BigInt& other)
    {
        BigInt tmp(*this);
        tmp = tmp - other;
        return tmp;
    }
    
    BigInt& operator++()
    {
        *this += 1;
        return *this;
    }
    
    BigInt operator++(int)
    {
        BigInt tmp(*this);
        tmp+=1;
        return tmp;
    }
    
    BigInt& operator--()
    {
        *this -= 1;
        return *this;
    }
    
    BigInt operator--(int)
    {
        BigInt tmp(*this);
        tmp-=1;
        return tmp;
    }
    
    bool sign()
    {
        return nonn;
    }
    
    bool operator== (const BigInt n)
    {
        BigInt t1 = n;
        BigInt t2 = *this;
        t1.brief();
        t2.brief();
        if (t1.quot == nullptr)
        {
            return (t1.res == t2.res)&&(t2.quot == nullptr)&&((t1.nonn == t2.nonn)||(t1.res == 0));
        }
        if (t2.quot == nullptr)
        {
            return (t1.res == t2.res)&&(t1.quot == nullptr);
        }
        return (*t1.quot == *t2.quot);
    }
    
    bool operator!= (const BigInt n)
    {
        return !(*this == n);
    }
    
    bool operator> (const BigInt n)
    {
        BigInt temp = *this - n;
        return temp.sign()&&(!(*this == n));
    }
    
    bool operator< (const BigInt n)
    {
        BigInt temp = *this - n;
        return !temp.sign();
    }
    
    bool operator>= (const BigInt n)
    {
        BigInt temp = *this - n;
        return temp.sign();
    }
    
    bool operator<= (const BigInt n)
    {
        BigInt temp = *this - n;
        return (!temp.sign())||((*this == n));
    }
    
    BigInt operator* (const BigInt& other) const
    {
        BigInt temp = BigInt(0);
        for (BigInt i = BigInt(0); i < *this; i++)
        {
            temp += other;
        }
        return temp;
    }
    
    BigInt operator*=(const BigInt& other)
    {
        BigInt tmp(*this);
        tmp = tmp * other;
        return tmp;
    }

    friend std::ostream& operator<<(std::ostream& out, const BigInt& value);
};

std::ostream& operator<< (std::ostream& out, const BigInt& value)
{
    BigInt temp = value;
    temp.brief();
    if (!temp.nonn)
    {
        out << "-";
    }
    if (temp.quot != nullptr)
    {
        temp.quot->nonn = true;
        out << *(temp.quot);
    }
    if ((temp.res >= 10000)||(temp.quot == nullptr))
    {
        out << temp.res;
    }
    else
    {
        if (temp.res >= 1000)
        {
            out << "0" << temp.res;
        }
        else
        {
            if (temp.res >= 100)
            {
                out << "00" << temp.res;
            }
            else
            {
                if (temp.res >= 10)
                {
                    out << "000" << temp.res;
                }
                else
                {
                    out << "0000" << temp.res;
                }
            }
        }
    }
    return out;
}

#endif
