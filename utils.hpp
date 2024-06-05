#include <vector>
#include <iostream>
#include <string>
#include <algorithm>

template <typename T>
bool contains(const std::vector<T>& vec, T value) {
    return std::find(vec.begin(), vec.end(), value) != vec.end();
}

template <typename T>
int findIndex(const std::vector<T>& vec, T value) {
    for (int i = 0; i < vec.size(); ++i)
        if (vec[i] == value)
            return i;
    return -1;
}

template <typename T>
void printVector(std::vector<T>& vec, const std::string& end = "\n") {
    std::cout << "[ ";
    for (const auto& elem : vec)
        std::cout << elem << " ";
    std::cout << "]";
}

template <typename T>
void printList(std::list<T>& vec, const std::string& end = "\n") {
    std::cout << "[ ";
    for (const auto& elem : vec)
        std::cout << elem << " ";
    std::cout << "]";
}

template <typename T>
void printArray(T array[], int size, const std::string& end = "\n") {
    std::cout << "[ ";
    for (int i = 0; i < size; ++i)
        std::cout << array[i] << " ";
    std::cout << "]" << end;
}