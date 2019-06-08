#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

class Semaphore
{
    std::mutex mutex_;
    std::condition_variable condition_;
    int counter_;
public:
    explicit Semaphore(int initialValue = 1)
    : counter_(initialValue)
    {
    }
    
    void enter()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock,
                        [this]()
                        {
                            return counter_ > 0;
                        });
        --counter_;
    }
    
    void leave()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        ++counter_;
        condition_.notify_one();
    }
};

Semaphore pingsem; 
Semaphore pongsem;

void pingfunc()
{
    pongsem.enter();
    for(int i = 0; i < 500000; i++)
    {
        pingsem.enter();
        std::cout << "ping \n";
        pongsem.leave();
    }
}

void pongfunc()
{
    for(int i = 0; i < 500000; i++)
    {
        pongsem.enter();
        std::cout << "pong \n";
        pingsem.leave();
    }
}

int main()
{
    std::thread pingthread(pingfunc);
    std::thread pongthread(pongfunc);
    pingthread.join();
    pongthread.join();
}
