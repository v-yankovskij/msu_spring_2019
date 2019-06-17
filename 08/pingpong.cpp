#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

#define A 500000

void pingfunc(bool& ping_, std::mutex& mut)
{
    for (int i = 0; i < A; i++)
    {
        mut.lock();
        if (ping_)
        {
            std::cout << "ping\n";
            ping_ = false;
        }
        else
        {
            i--;
        }
        mut.unlock();
    }
}

void pongfunc(bool& ping_, std::mutex& mut)
{
    for (int i = 0; i < A; i++)
    {
        mut.lock();
        if (!ping_)
        {
            std::cout << "ping\n";
            ping_ = true;
        }
        else
        {
            i--;
        }
        mut.unlock();
    }
}

int main()
{
    std::mutex mut;
    bool ping_ = true;
    std::vector<std::thread> threads;
    threads.emplace_back(pingfunc, std::ref(ping_), std::ref(mut));
    threads.emplace_back(pongfunc, std::ref(ping_), std::ref(mut));
    threads[0].join();
    threads[1].join();
}
