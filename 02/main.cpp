#include <iostream>
#include <string>
    
std::int64_t calculate (const std::string& expression, bool& error)
{
    using namespace std;
    int64_t answer;
    int64_t current_monome;
    int64_t current_number;
    bool negative;
    bool new_number;
    bool inverse;
    answer = 0;
    current_monome = 1;
    current_number = 0;
    negative = false;
    new_number = true;
    inverse = false;
    for (int i = 0; i < expression.length(); i++)
    {
        if (expression[i] != ' ')
        {
            if ((expression[i] <= '9') && (expression[i] >= '0'))
            {
                new_number = false;
                current_number *= 10;
                current_number += expression[i] - '0';
            }
            else
            {
                if ((expression[i] == '-')&&(new_number))
                {
                    negative = !negative;
                }
                else
                {
                    if ((expression[i] == '*')||(expression[i] == '+')||(expression[i] == '/')||(expression[i] == '-'))
                    {
                        if (new_number)
                        {
                            error = true;
                            return 1;
                        }
                        if (negative)
                        {
                            current_number = -current_number;
                        }
                        if (inverse)
                        {
                            if (current_number == 0)
                            {
                                error = true;
                                return 1;
                            }
                            current_monome /= current_number;
                        }
                        else
                        {
                            current_monome *= current_number;
                        }
                        current_number = 0;
                        if (expression[i] == '*')
                        {
                            new_number = true;
                            negative = false;
                            inverse = false;
                        }
                        if (expression[i] == '/')
                        {
                            new_number = true;
                            negative = false;
                            inverse = true;
                        }
                        if (expression[i] == '+')
                        {
                            answer += current_monome;
                            current_monome = 1;
                            new_number = true;
                            negative = false;
                            inverse = false;
                        }
                        if (expression[i] == '-')
                        {
                            answer += current_monome;
                            current_monome = 1;
                            new_number = true;
                            negative = true;
                            inverse = false;
                        }
                    }
                    else
                    {
                        error = true;
                        return 1;
                    }
                }
            }
        }
    }
    if (new_number)
    {
        error = true;
        return 1;
    }
    if (negative)
    {
        current_number = -current_number;
    }
    if (inverse)
    {
        if (current_number == 0)
        {
            error = true;
            return 1;
        }
        current_monome /= current_number;
    }
    else
    {
        current_monome *= current_number;
    }
    answer += current_monome;
    error = false;
    return answer;
}

int main (int argc, char** argv)
{
    using namespace std;
    bool error;
    int64_t answer;
    string expression;
    if (argc != 2)
    {
        cout << "error";
        return 1;
    }
    expression = argv[1];
    error = false;
    answer = calculate(expression, error);
    if (error)
    {
        cout << "error";
        return 1;
    }
    cout << answer;
    return 0;
}		
