#pragma once

#include <cmath>
#include <algorithm>

namespace Eigen {

// Forward declarations
template<typename T>
class Matrix;

template<typename T>
class Vector;

typedef Matrix<float> MatrixXf;
typedef Vector<float> VectorXf;

// Simple matrix implementation for polynomial fitting
template<typename T>
class Matrix {
public:
    Matrix() : rows_(0), cols_(0), data_(nullptr) {}
    
    Matrix(int rows, int cols) : rows_(rows), cols_(cols) {
        data_ = new T[rows * cols];
        for (int i = 0; i < rows * cols; i++) {
            data_[i] = T(0);
        }
    }
    
    ~Matrix() {
        if (data_) delete[] data_;
    }
    
    // Copy constructor
    Matrix(const Matrix& other) : rows_(other.rows_), cols_(other.cols_) {
        data_ = new T[rows_ * cols_];
        for (int i = 0; i < rows_ * cols_; i++) {
            data_[i] = other.data_[i];
        }
    }
    
    // Assignment operator
    Matrix& operator=(const Matrix& other) {
        if (this != &other) {
            if (data_) delete[] data_;
            rows_ = other.rows_;
            cols_ = other.cols_;
            data_ = new T[rows_ * cols_];
            for (int i = 0; i < rows_ * cols_; i++) {
                data_[i] = other.data_[i];
            }
        }
        return *this;
    }
    
    // Element access
    T& operator()(int row, int col) {
        return data_[row * cols_ + col];
    }
    
    const T& operator()(int row, int col) const {
        return data_[row * cols_ + col];
    }
    
    // Accessor methods for dimensions
    int rows() const { return rows_; }
    int cols() const { return cols_; }
    
    // Matrix transpose
    Matrix transpose() const {
        Matrix result(cols_, rows_);
        for (int i = 0; i < rows_; i++) {
            for (int j = 0; j < cols_; j++) {
                result(j, i) = (*this)(i, j);
            }
        }
        return result;
    }
    
    // Matrix multiplication
    Matrix operator*(const Matrix& other) const {
        if (cols_ != other.rows_) {
            // Error - incompatible matrices
            return Matrix(0, 0);
        }
        
        Matrix result(rows_, other.cols_);
        for (int i = 0; i < rows_; i++) {
            for (int j = 0; j < other.cols_; j++) {
                T sum = T(0);
                for (int k = 0; k < cols_; k++) {
                    sum += (*this)(i, k) * other(k, j);
                }
                result(i, j) = sum;
            }
        }
        return result;
    }
    
    // QR decomposition for solving least squares
    class HouseholderQR {
    public:
        HouseholderQR(const Matrix& matrix) : m_(matrix) {
            // Simplified QR implementation
        }
        
        Vector<T> solve(const Vector<T>& b) const {
            // For polynomial fitting, use normal equations (A^T * A) * x = A^T * b
            Matrix ATA = m_.transpose() * m_;
            Vector<T> ATb = m_.transpose() * b;
            
            // Solve using Gaussian elimination
            int n = ATA.cols();
            Vector<T> x(n);
            
            // Create augmented matrix [ATA | ATb]
            Matrix<T> aug(n, n + 1);
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    aug(i, j) = ATA(i, j);
                }
                aug(i, n) = ATb(i);
            }
            
            // Gaussian elimination with partial pivoting
            for (int i = 0; i < n; i++) {
                // Find pivot
                int pivot = i;
                for (int j = i + 1; j < n; j++) {
                    if (std::abs(aug(j, i)) > std::abs(aug(pivot, i))) {
                        pivot = j;
                    }
                }
                
                // Swap rows
                if (pivot != i) {
                    for (int j = i; j <= n; j++) {
                        T temp = aug(i, j);
                        aug(i, j) = aug(pivot, j);
                        aug(pivot, j) = temp;
                    }
                }
                
                // Eliminate below
                for (int j = i + 1; j < n; j++) {
                    T factor = aug(j, i) / aug(i, i);
                    aug(j, i) = 0;
                    for (int k = i + 1; k <= n; k++) {
                        aug(j, k) -= factor * aug(i, k);
                    }
                }
            }
            
            // Back substitution
            for (int i = n - 1; i >= 0; i--) {
                T sum = 0;
                for (int j = i + 1; j < n; j++) {
                    sum += aug(i, j) * x(j);
                }
                x(i) = (aug(i, n) - sum) / aug(i, i);
            }
            
            return x;
        }
        
    private:
        Matrix m_;
    };
    
    HouseholderQR householderQr() const {
        return HouseholderQR(*this);
    }
    
private:
    int rows_;
    int cols_;
    T* data_;
    
    // Allow Vector class and friend functions to access private members
    template<typename U> friend class Vector;
    template<typename U> friend Vector<U> operator*(const Matrix<U>&, const Vector<U>&);
};

// Simplified vector class (1D matrix)
template<typename T>
class Vector {
public:
    Vector() : size_(0), data_(nullptr) {}
    
    Vector(int size) : size_(size) {
        data_ = new T[size];
        for (int i = 0; i < size; i++) {
            data_[i] = T(0);
        }
    }
    
    ~Vector() {
        if (data_) delete[] data_;
    }
    
    // Copy constructor
    Vector(const Vector& other) : size_(other.size_) {
        data_ = new T[size_];
        for (int i = 0; i < size_; i++) {
            data_[i] = other.data_[i];
        }
    }
    
    // Assignment operator
    Vector& operator=(const Vector& other) {
        if (this != &other) {
            if (data_) delete[] data_;
            size_ = other.size_;
            data_ = new T[size_];
            for (int i = 0; i < size_; i++) {
                data_[i] = other.data_[i];
            }
        }
        return *this;
    }
    
    // Element access
    T& operator()(int index) {
        return data_[index];
    }
    
    const T& operator()(int index) const {
        return data_[index];
    }
    
    // Get vector size
    int size() const { return size_; }
    
    // Vector as a matrix
    operator Matrix<T>() const {
        Matrix<T> result(size_, 1);
        for (int i = 0; i < size_; i++) {
            result(i, 0) = data_[i];
        }
        return result;
    }
    
private:
    int size_;
    T* data_;
    
    // Allow Matrix class and friend functions to access private members
    template<typename U> friend class Matrix;
    template<typename U> friend Vector<U> operator*(const Matrix<U>&, const Vector<U>&);
};

// Matrix * Vector multiplication
template<typename T>
Vector<T> operator*(const Matrix<T>& matrix, const Vector<T>& vector) {
    if (matrix.cols_ != vector.size_) {
        // Error - incompatible dimensions
        return Vector<T>(0);
    }
    
    Vector<T> result(matrix.rows_);
    for (int i = 0; i < matrix.rows_; i++) {
        T sum = T(0);
        for (int j = 0; j < matrix.cols_; j++) {
            sum += matrix(i, j) * vector(j);
        }
        result(i) = sum;
    }
    return result;
}

} // namespace Eigen