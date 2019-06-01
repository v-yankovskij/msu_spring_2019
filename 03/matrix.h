#ifndef matrix_h
#define matrix_h

#include <vector>

class Row
{
public:
    size_t cols;
    std::vector<double> data_;
    
    Row (size_t a)
    {
        cols = a;
        for (int i = 0; i < a; i++)
        {
            data_.push_back(0);
        }
    }
    
    const double& operator[](size_t j) const
    {
        if ((j >= 0) && (j < cols))
        {
            return data_[j];
        }
        else
        {
            throw std::out_of_range("");
        }
    }
    
    double& operator[](size_t j)
    {
        if ((j >= 0) && (j < cols))
        {
            return data_[j];
        }
        else
        {
            throw std::out_of_range("");
        }
    }
};

class Matrix
{
public:
    size_t rows;
    size_t cols;
    std::vector<Row> data;
    
    Matrix (size_t a, size_t b)
    {
        rows = a;
        cols = b;
        for (int i = 0; i < a; i++)
        {
            data.push_back(Row(cols));
        }
    }
    
    const Row& operator[](size_t i) const
    {
        if ((i < 0) || (i >= rows))
        {
            throw std::out_of_range("");
        }
        return data[i];
    }
    
    Row& operator[](size_t i)
    {
        if ((i < 0) || (i >= rows))
        {
            throw std::out_of_range("");
        }
        return data[i];
    }
    
    size_t getRows()
    {
        return rows;
    }
    
    size_t getColumns()
    {
        return cols;
    }
    
    void operator*= (double a)
    {
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                data[i][j] *= a;
            }
        }
    }
    
    bool operator== (Matrix M)
    {
        if (cols != M.getColumns())
        {
            return false;
        }
        if (rows != M.getRows())
        {
            return false;
        }
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                if (data[i][j] != M[i][j])
                {
                    return false;
                }
            }
        }
        return true;
    }

    bool operator!= (Matrix M)
    {
        if (cols != M.getColumns())
        {
            return true;
        }
        if (rows != M.getRows())
        {
            return true;
        }
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                if (data[i][j] != M[i][j])
                {
                    return true;
                }
            }
        }
        return false;
    }
};


#endif
