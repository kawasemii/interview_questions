#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <utility>  // For std::forward




template <typename T>
class MyVector {
private:
    T* data;
    size_t capacity;
    size_t size;

    template <class U=T, std::enable_if_t<std::is_nothrow_move_constructible_v<U>, bool> =true>
    void increaseCapacity(size_t newCapacity) {
        T* newData = new T[newCapacity];
        std::move(data, data+size, newData);
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }
    
    template <class U=T, std::enable_if_t<!std::is_nothrow_move_constructible_v<U>, bool> =true>
    void increaseCapacity(size_t newCapacity) {
        T* newData = new T[newCapacity];
        std::copy(data, data+size, newData);
        delete[] data;
        data = newData;
        capacity = newCapacity;
        std::cout<<"copy to expand capacity" << std::endl;
    }

public:
    MyVector() : data(nullptr), capacity(1), size(0) {
        data = new T[capacity];
    }

    MyVector(size_t count, const T& value) : capacity(count), size(count) {
        data = new T[capacity];
        for (size_t i = 0; i < size; ++i) {
            data[i] = value;
        }
    }

    ~MyVector() {
        delete[] data;
    }



    void push_back(const T& value) {
        if (size == capacity) {
            increaseCapacity(capacity * 2);
        }
        data[size++] = value;
    }
    
    void push_back(T&& value) {
        if (size == capacity) {
            increaseCapacity(capacity * 2);
        }
        data[size++] = std::move(value);
    }


    // 模板参数包
    template <typename... Args>
    void emplace_back(Args&&... args) {     //函数参数包
        if (size == capacity) {
            increaseCapacity(capacity * 2);
        }
        new(&data[size++]) T(std::forward<Args>(args)...);
    }


    T& operator[](size_t index) {
        if (index >= size) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    size_t getSize() const {
        return size;
    }

    bool isEmpty() const {
        return size == 0;
    }

    void resize(size_t newSize) {
        if (newSize > capacity) {
            increaseCapacity(newSize);
        }
        size = newSize;
    }

    void reverse() {
        std::reverse(data, data + size);
    }

    void shrink_to_fit() {
        if (size < capacity) {
            increaseCapacity(size);
        }
    }

    void reserve(size_t newCapacity) {
        if (newCapacity > capacity) {
            increaseCapacity(newCapacity);
        }
    }
};


struct NoMove {
    NoMove() = default;
    NoMove(NoMove const &) = default;
    NoMove& operator=(NoMove &) = default;
    
    NoMove (NoMove&& other) noexcept(false) : member(std::move(other.member)) { }
    
    NoMove& operator=(NoMove&& other) noexcept(false) {
        if(this != &other) {
            member= std::move(other.member);
        }
        return *this;
    }
    
    NoMove(int x):member(std::move(x)){}
    
    int member = 100;
    
    friend auto& operator<<(std::ostream& os, NoMove const & ms) {
    	os << ms.member; 
    	return os;
    }
};


int main() {
    MyVector<int> v1(2, 3);

    MyVector<std::pair<int, std::string>> vec;
    vec.emplace_back(1, "one");
    vec.emplace_back(2, "two");
    vec.emplace_back(3, "three");

    for (size_t i = 0; i < vec.getSize(); ++i) {
        std::cout << "(" << vec[i].first << ", " << vec[i].second << ") ";
    }
    std::cout << std::endl;

    MyVector<NoMove> v2;
    v2.push_back(5);
    v2.push_back(NoMove(50));
    for (size_t i = 0; i < v2.getSize(); ++i) {
        std::cout << v2[i] << " ";
    }
    
    return 0;
}
