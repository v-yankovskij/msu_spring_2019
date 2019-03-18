#include "numbers.dat"
#include <iostream>

bool is_prime(int p)
{
	if (p == 1)
	{
		return false;
	}
	for(int i = 2; i*i <= p; i++)
	{
		if (p % i == 0)	
		{
			return false;
		}
	}
	return true;
}

int count_primes(int a, int b)
{
	if (a > b)
	{
		return 0;
	}
	int current;
	int number_of_primes;
	bool test;
	current = 0;
	number_of_primes = 0;
	while ((current < Size) && (Data[current] < a))
	{
		current++;
	}
	if ((current == Size) && (Data[Size - 1] != b))
	{
		return 0;
	}
	if (Data[current] > a)
	{
		return 0;
	}
	while ((current < Size) && (Data[current] <= b))
	{
		if ((current == 0) || ((current > 0) && (Data[current - 1] != Data[current])))
		{
			test = is_prime(Data[current]);
		}
		if (test)
		{
			number_of_primes++;
		}
		current++;
	}
	if ((current == Size) && (Data[Size - 1] != b))
	{
		return 0;
	}
	if (Data[current - 1] < b)
	{
		return 0;
	}
	return number_of_primes;
}

int main(int argc, char* argv[])
{
	using namespace std;
    if (argc % 2 == 0)
    {
    	return -1;
    }
    if (argc == 1)
    {
    	return -1;
    }
    for (int i = 1; i < argc; i += 2)
    {
    	cout << count_primes(atoi(argv[i]), atoi(argv[i + 1])) << '\n';
    }
    return 0;
}
