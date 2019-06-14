#include <iostream>
#include <fstream>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <string>
#include <algorithm>

std::atomic<bool> finished(false);
std::atomic<int> finished_step(0);
std::atomic<int> finished_sort(0);
std::condition_variable condition;
std::mutex sort_finish_mutex;
std::mutex iter_finish_mutex;
std::mutex stream_read_mutex;
std::mutex output_queue_mutex;

void split(uint64_t * numbers, std::ifstream & datas, int id, std::queue<std::string>& output_queue)
{
    int snum = 0;
    while (!datas.eof())
    {
        std::unique_lock<std::mutex> lock(stream_read_mutex);
        datas.read(reinterpret_cast<char *>(numbers), 3145728);
        std::streamsize current_size = datas.gcount() / sizeof(uint64_t);
        lock.unlock();
        if (current_size != 0)
        {
            std::sort(numbers, numbers + current_size);
            std::string name = std::to_string(0) + "." + std::to_string(id) + "." + std::to_string(snum) + ".bin";
            std::ofstream output_stream(name, std::ios::binary);
            output_stream.write(reinterpret_cast<char*>(numbers), current_size * sizeof(uint64_t));
            output_queue.push(name);
            snum++;
        }
    }
    
}

void merge(std::string& string1, std::string& string2, uint64_t* num, int id, int i, int snum, std::queue<std::string>&output_queue)
{
    std::ifstream stream1(string1, std::ios::binary);
    std::ifstream stream2(string2, std::ios::binary);
    std::string name = std::to_string(i) + '.' + std::to_string(id) + '.' + std::to_string(snum) + ".bin";
    std::ofstream output_stream(name, std::ios::binary);
    uint64_t* left = num;
    uint64_t* right = num + 786432;
    uint64_t* centre = centre + 786432;
    stream1.read(reinterpret_cast<char*>(left), 786432 * sizeof(uint64_t));
    size_t read_left = stream1.gcount() / sizeof(uint64_t);
    stream2.read(reinterpret_cast<char*>(right), 786432 * sizeof(uint64_t));
    size_t read_right = stream2.gcount() / sizeof(uint64_t);
    size_t l = 0;
    size_t c = 0;
    size_t r = 0;
    while (!stream1.eof() || !stream2.eof() || l < read_left || c < read_right)
    {
        if ((l == read_left) && !stream1.eof())
        {
            stream1.read(reinterpret_cast<char*>(left), 786432 * sizeof(uint64_t));
            read_left = stream1.gcount() / sizeof(uint64_t);
            l = 0;
        }
        if ((r == read_right) && !stream2.eof())
        {
            stream2.read(reinterpret_cast<char*>(right), 786432 * sizeof(uint64_t));
            read_right = stream2.gcount() / sizeof(uint64_t);
            c = 0;
        }
        if (c == 1572864)
        {
            output_stream.write(reinterpret_cast<char*>(centre), c * sizeof(uint64_t));
            c = 0;
        }
        if (l < read_left && r < read_right)
        {
            if (right[r] < left[l])
            {
                centre[c] = right[r];
                r++;
                c++;
            }
            else
            {
                centre[c] = left[l];
                l++;
                c++;
            }
        }
        else
        {
            if (l == read_left && r < read_right)
            {
                centre[c] = right[r];
                r++;
                c++;
            }
            else
            {
                if (r == read_right && l < read_left)
                {
                    centre[c] = left[l];
                    l++;
                    c++;
                }
            }
        }
    }
    output_stream.write(reinterpret_cast<char*>(centre), c * sizeof(uint64_t));
    std::unique_lock<std::mutex> qlock(output_queue_mutex);
    output_queue.push(name);
}

void Sort(uint64_t * nums, std::ifstream & datas, int id, std::queue<std::string>& output_queue)
{
    uint64_t * numbers = nums + id * 3145728 / sizeof(uint64_t);
    int i = 0;
    int snum = 0;
    split(numbers, datas, id, output_queue);
    i++;
    std::unique_lock<std::mutex> lock(iter_finish_mutex);
    finished_step++;
    condition.notify_all();
    while (finished_step < 2)
    {
        condition.wait(lock);
    }
    lock.unlock();
    while (output_queue.size() >= 2)
    {
        std::unique_lock<std::mutex> qLock(output_queue_mutex);
        std::string tmp1 = output_queue.front();
        output_queue.pop();
        std::string tmp2 = output_queue.front();
        output_queue.pop();
        qLock.unlock();
        merge(tmp1, tmp2, numbers, id, i, snum, output_queue);
        snum++;
    }
    std::unique_lock<std::mutex> fLock(sort_finish_mutex);
    finished_sort++;
    if (finished_sort == 2)
    {
        if (output_queue.empty())
        {
            std::cerr << "error, no output files" << std::endl;
        }
        else
        {
            std::cout << output_queue.front() << std::endl;
        }
    }
}

int main()
{
    uint64_t * nums = new uint64_t [6291456 / sizeof(uint64_t)];
    std::ifstream datas("data.bin", std::ios::binary);
    std::queue<std::string> output_queue;
    std::vector<std::thread> threads;
    if (!datas)
    {
        throw std::invalid_argument("can't open file");
    }
    threads.emplace_back(Sort, std::ref(nums),std::ref(datas), 0, std::ref(output_queue));
    threads.emplace_back(Sort, std::ref(nums),std::ref(datas), 1, std::ref(output_queue));
    threads[0].join();
    threads[1].join();
    delete[] nums;
    system("pause");
    return 0;
}
