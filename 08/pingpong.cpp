#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

#define A 500000

std::mutex mut;
bool ping_ = true;

void pingfunc()
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

void pongfunc()
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
    std::thread pingthread(pingfunc);
    std::thread pongthread(pongfunc);
    pingthread.join();
    pongthread.join();
}
