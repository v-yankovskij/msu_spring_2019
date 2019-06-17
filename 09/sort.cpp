#include <iostream>
#include <fstream>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <string>
#include <algorithm>

#define M 6291456
#define N 2
#define m M/N

class uint_seq
{
public:
    size_t size;
    uint64_t* numbers;

    uint_seq (size_t n)
    {
        numbers = new uint64_t [n];
    }

    const uint64_t& operator[](size_t i) const
    {
        if ((i >= 0) && (i < size))
        {
            return numbers[i];
        }
        else
        {
            throw std::out_of_range("");
        }
    }

    uint64_t& operator[](size_t i)
    {
        if ((i >= 0) && (i < size))
        {
            return numbers[i];
        }
        else
        {
            throw std::out_of_range("");
        }
    }

    ~uint_seq()
    {
        delete[] numbers;
    }
};

void split(uint64_t * numbers, std::ifstream& datas, int id, std::queue<std::string>& output_queue, std::mutex& stream_read_mutex)
{
    int snum = 0;
    while (!datas.eof())
    {
        std::unique_lock<std::mutex> lock(stream_read_mutex);
        datas.read(reinterpret_cast<char *>(numbers), m);
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

void merge(std::string& string1, std::string& string2, uint64_t* numbers, int id, int i, int snum, std::queue<std::string>&output_queue, std::mutex& output_queue_mutex)
{
    std::ifstream stream1(string1, std::ios::binary);
    std::ifstream stream2(string2, std::ios::binary);
    std::string name = std::to_string(i) + '.' + std::to_string(id) + '.' + std::to_string(snum) + ".bin";
    std::ofstream output_stream(name, std::ios::binary);
    uint64_t* left = numbers;
    uint64_t* right = numbers + (m/(4*sizeof(uint64_t)));
    uint64_t* centre = centre + (m/(4*sizeof(uint64_t)));
    stream1.read(reinterpret_cast<char*>(left), m/4);
    size_t read_left = stream1.gcount() / sizeof(uint64_t);
    stream2.read(reinterpret_cast<char*>(right), m/4);
    size_t read_right = stream2.gcount() / sizeof(uint64_t);
    size_t l = 0;
    size_t c = 0;
    size_t r = 0;
    while (!stream1.eof() || !stream2.eof() || l < read_left || c < read_right)
    {
        if ((l == read_left) && !stream1.eof())
        {
            stream1.read(reinterpret_cast<char*>(left), m/4);
            read_left = stream1.gcount() / sizeof(uint64_t);
            l = 0;
        }
        if ((r == read_right) && !stream2.eof())
        {
            stream2.read(reinterpret_cast<char*>(right), m/4);
            read_right = stream2.gcount() / sizeof(uint64_t);
            c = 0;
        }
        if (c == m/(2*sizeof(uint64_t)))
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

void Sort(uint_seq& nums, std::ifstream & datas, int id, std::queue<std::string>& output_queue, std::atomic<bool>& finished, std::atomic<int>& finished_step, std::atomic<int>& finished_sort, std::condition_variable& condition, std::mutex& sort_finish_mutex, std::mutex& iter_finish_mutex, std::mutex& stream_read_mutex, std::mutex& output_queue_mutex)
{
    uint64_t * numb = nums.numbers + id * m / sizeof(uint64_t);
    int i = 0;
    int snum = 0;
    split(numb, datas, id, output_queue, stream_read_mutex);
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
        merge(tmp1, tmp2, numb, id, i, snum, output_queue, output_queue_mutex);
        snum++;
    }
    std::unique_lock<std::mutex> fLock(sort_finish_mutex);
    finished_sort++;
    if (finished_sort == N)
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
    std::atomic<bool> finished(false);
    std::atomic<int> finished_step(0);
    std::atomic<int> finished_sort(0);
    std::condition_variable condition;
    std::mutex sort_finish_mutex;
    std::mutex iter_finish_mutex;
    std::mutex stream_read_mutex;
    std::mutex output_queue_mutex;
    uint_seq nums(M / sizeof(uint64_t));
    std::ifstream datas("data.bin", std::ios::binary);
    std::queue<std::string> output_queue;
    std::vector<std::thread> threads;
    if (!datas)
    {
        throw std::invalid_argument("can't open file");
    }
    for (int i = 0; i < N; i++)
    {
        threads.emplace_back(Sort, std::ref(nums),std::ref(datas), i, std::ref(output_queue), std::ref(finished), std::ref(finished_step), std::ref(finished_sort), std::ref(condition), std::ref(sort_finish_mutex), std::ref(iter_finish_mutex), std::ref(stream_read_mutex), std::ref(output_queue_mutex));
    }
    for (int i = 0; i < N; i++)
    {
        threads[i].join();
    }
    system("pause");
    return 0;
}
