#ifndef vector_h
#define vector_h

#include <iterator>

template <class T>
class Allocator
{
public:
    using value_type = T;
    using pointer = T*;
    using size_type = size_t;
    
    pointer allocate(size_type n)
    {
        return static_cast<pointer>(::operator new(n * sizeof(value_type)));
    }
    
    void deallocate(pointer p, size_type n)
    {
        ::operator delete(p, n * sizeof(value_type));
    }
    
    template <class... args> void construct(pointer point, args... Args)
    {
        ::new(point) value_type(Args...);
    }
    
    void destroy(pointer point)
    {
        point->~value_type();
    }
};

template <class T>
class Iterator : public std::iterator<std::random_access_iterator_tag, T>
{
public:
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator = Iterator<value_type>;
    pointer current;

    explicit Iterator(pointer p) : current(p) {}
    
    Iterator(const iterator& iter) : current(iter.current) {}
    
    bool operator==(const iterator& other)
    {
        return current == other.current;
    }
    
    bool operator!=(const iterator& other)
    {
        return !(*this == other);
    }
    
    bool operator<(const iterator& other)
    {
        return current < other.current_;
    }
    
    bool operator>(const iterator& other)
    {
        return other < *this;
    }
    
    bool operator>=(const iterator& other)
    {
        return !(*this < other);
    }
    
    bool operator<=(const iterator& other)
    {
        return !(*this > other);
    }
    
    reference operator*()
    {
        return *current;
    }
    
    iterator& operator++()
    {
        current++;
        return *this;
    }
    
    iterator& operator--()
    {
        current--;
        return *this;
    }
    
    iterator& operator+=(size_t n)
    {
        current += n;
        return *this;
    }
    
    iterator& operator-=(size_t n)
    {
        current -= n;
        return *this;
    }
    
    iterator operator+(size_t n)
    {
        iterator tmp(current + n);
        return tmp;
    }
    
    iterator operator-(size_t n)
    {
        iterator tmp(current - n);
        return tmp;
    }
    
    iterator operator+(const iterator& other)
    {
        iterator tmp(current + other.current);
        return tmp;
    }
    
    iterator operator-(const iterator& other)
    {
        iterator tmp(current - other.current);
        return tmp;
    }
    
    friend iterator operator+(size_t n, const iterator& other)
    {
        iterator tmp(other.current + n);
        return tmp;
    }
    
    friend iterator operator-(size_t n, const iterator& other)
    {
        iterator tmp(other.current - n);
        return tmp;
    }
    
    reference operator[](size_t n)
    {
        return current[n];
    }
};

template <class T, class Alloc = Allocator<T>>
class Vector
{
public:
    using iterator = Iterator<T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using allocator_type = Alloc;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    Alloc alloc;
    pointer data;
    size_type Size;
    size_type allocated_size;

    explicit Vector(size_type n = 0) : data(alloc.allocate(n)), Size(n), allocated_size(n)
    {
        for (size_type i = 0; i < Size; i++)
        {
            alloc.construct(data + i);
        }
    }
    
    ~Vector()
    {
        clear();
        alloc.deallocate(data, allocated_size);
    }
    
    reference operator[] (size_type i)
    {
        return data[i];
    }
    
    const_reference operator[] (size_type i) const
    {
        return data[i];
    }
    
    void pop_back()
    {
        Size--;
        alloc.destroy(data + Size);
    }
    
    void push_back(value_type &&value)
    {
        if (Size >= allocated_size)
        {
            reserve(2 * allocated_size + 1);
        }
        alloc.construct(data + Size, std::move(value));
        Size++;
    }
    
    void push_back(const value_type& value)
    {
        if (Size >= allocated_size)
        {
            reserve(2 * allocated_size + 1);
        }
        alloc.construct(data + Size, value);
        size++;
    }
    
    size_type size() const
    {
        return Size;
    }
    
    size_type capacity()
    {
        return allocated_size;
    }
    
    bool empty() const
    {
        return (Size == 0);
    }
    
    iterator begin()
    {
        return iterator(data);
    }
    
    iterator end()
    {
        return iterator(data + Size);
    }
    
    reverse_iterator rbegin()
    {
        return reverse_iterator(end());
    }
    
    reverse_iterator rend()
    {
        return reverse_iterator(begin());
    }
    
    void reserve(size_type new_cap)
    {
        if (new_cap <= Size)
        {
            return;
        }
        pointer tmp = alloc.allocate(new_cap);
        for (size_type i = 0; i < Size; i++)
        {
            alloc.construct(tmp + i, data[i]);
            alloc.destroy(data + i);
        }
        alloc.deallocate(data, allocated_size);
        data = tmp;
        allocated_size = new_cap;
    }
    
    void clear()
    {
        while (Size > 0)
        {
            alloc.destroy(data + Size);
            Size--;
        }
    }
    
    void resize(size_type n)
    {
        if (n < Size)
        {
            for (pointer p = data + n; p != data + Size; p++)
            {
                alloc.destroy(p);
            }
            Size = n;
            return;
        }
        if(n >= allocated_size)
        {
            this->reserve(n);
        }
        for (pointer p = data + Size; p != data + n; p++)
        {
            alloc.construct(p);
        }
        Size = n;
        return;
    }
};

#endif
