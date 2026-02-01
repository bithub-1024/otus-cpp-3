
#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <memory>
#include <cstddef>
#include <type_traits>
#include <typeinfo>
#include <cstdlib>
#include <string>

// АЛЛОКАТОР 
template <typename T, size_t init_size = 10>
class allocator {
private:
    T* data;
    allocator* next_block;
    bool block_allocated;
    size_t used;
    size_t capacity;
    size_t block_id;
    static size_t total_blocks;
    
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    
    template<typename U>
    struct rebind {
        using other = allocator<U, init_size>;
    };
    
    allocator();
    template <typename U>
    allocator(const allocator<U, init_size>&);
    ~allocator();
    
    T* allocate(size_t n);
    void deallocate(T* p, size_t n) noexcept;
    
    template <typename U, typename... Args>
    void construct(U* p, Args&&... args);
    
    template<typename U>
    void destroy(U* p) noexcept;
    
    size_t max_size() const noexcept;
    
    template<typename U>
    bool operator==(const allocator<U, init_size>&) const noexcept;
    
    template<typename U>
    bool operator!=(const allocator<U, init_size>& other) const noexcept;
    
    void print_status() const;
    
    // Геттеры
    T* get_data() const { return data; }
    size_t get_used() const { return used; }
    size_t get_capacity() const { return capacity; }
};

// Инициализация статической переменной
template <typename T, size_t init_size>
size_t allocator<T, init_size>::total_blocks = 0;

// Реализация аллокатора
template <typename T, size_t init_size>
allocator<T, init_size>::allocator(): 
    data(nullptr), next_block(nullptr), block_allocated(false), 
    used(0), capacity(init_size), block_id(++total_blocks) 
{
    data = static_cast<T *>(malloc(sizeof(T) * init_size));
    if (!data) throw std::bad_alloc();
    else block_allocated = true;
    /*std::cout << "Выделена память для блока #" << block_id 
              << ": " << (capacity * sizeof(T)) << " байт\n";*/
}

template <typename T, size_t init_size>
template <typename U>
allocator<T, init_size>::allocator(const allocator<U, init_size>&) : 
    data(nullptr), next_block(nullptr), block_allocated(false), 
    used(0), capacity(init_size), block_id(++total_blocks) 
{
    data = static_cast<T *>(malloc(sizeof(T) * init_size));
    if (!data) throw std::bad_alloc();
    else block_allocated = true;
}

template <typename T, size_t init_size>
allocator<T, init_size>::~allocator()
{
    if (data && block_allocated) {
        free(data); 
        data = nullptr;
    }
    //if (next_block) delete next_block;
}

template <typename T, size_t init_size>
T* allocator<T, init_size>::allocate(size_t n) {
    if (n == 0) { return nullptr; }
    if (n > init_size)
    {
        std::cerr << "ОШИБКА: Запрос " << n << " превышает размер блока " << init_size << "\n";
        throw std::bad_alloc();
    }
    if (capacity < (used + n))
    {
        if (!next_block)
        {
            next_block = new allocator<T, init_size>();
        }
        return next_block->allocate(n);
    }
    else { 
        T* ptr;
        ptr = data + used;
        used += n;
        return ptr;
    }
}

template <typename T, size_t init_size>
void allocator<T, init_size>::deallocate(T* p, size_t n) noexcept {
    // STL требует этот метод
}

template <typename T, size_t init_size>
template <typename U, typename... Args>
void allocator<T, init_size>::construct(U* p, Args&&... args) {
    ::new((void*)p) U(std::forward<Args>(args)...);
}

template <typename T, size_t init_size>    
template<typename U>
void allocator<T, init_size>::destroy(U* p) noexcept {
    p->~U();
}

template <typename T, size_t init_size> 
size_t allocator<T, init_size>::max_size() const noexcept {
    return init_size;
}

template <typename T, size_t init_size>     
template<typename U>
bool allocator<T, init_size>::operator==(const allocator<U, init_size>&) const noexcept {
    return true;  
}

template <typename T, size_t init_size>    
template<typename U>
bool allocator<T, init_size>::operator!=(const allocator<U, init_size>& other) const noexcept {
    return !(*this == other);
}

template <typename T, size_t init_size> 
void allocator<T, init_size>::print_status() const {
    std::cout << "Блок #" << block_id << ": "
              << "использовано " << used << "/" << capacity 
              << " (" << (used * 100 / capacity) << "%), "
              << "адрес начала: " << data 
              << ", следующий блок: " << (next_block ? "есть" : "нет") << "\n";
}

// Оператор вывода
template <typename T, size_t N>
std::ostream& operator<<(std::ostream& os, const allocator<T, N>& alloc) {
    os << "allocator[блок #" << reinterpret_cast<const void*>(&alloc) 
       << ", data=" << alloc.get_data() 
       << ", used=" << alloc.get_used() 
       << "/" << alloc.get_capacity() << "]";
    return os;
}

// МОЙ КОНТЕЙНЕР 
template <typename T, typename Alloc = std::allocator<T>>
class MyContainer {
private:
    struct Node {
        T value;
        Node* next;
        Node(const T& val);
    };
    
    Node* head;
    Node* tail;
    size_t count;
    typename Alloc::template rebind<Node>::other alloc;
    
public:
    MyContainer();
    ~MyContainer();
    
    void add(const T& value);
    void clear();
    void print() const;
    size_t size() const;
    bool empty() const;
};

// Реализация MyContainer
template <typename T, typename Alloc>
MyContainer<T, Alloc>::Node::Node(const T& val) : value(val), next(nullptr) {}

template <typename T, typename Alloc>
MyContainer<T, Alloc>::MyContainer() : head(nullptr), tail(nullptr), count(0) {}

template <typename T, typename Alloc>
MyContainer<T, Alloc>::~MyContainer() {
    clear();
}

template <typename T, typename Alloc>
void MyContainer<T, Alloc>::add(const T& value) {
    Node* new_node = alloc.allocate(1);
    alloc.construct(new_node, value);
    
    if (!head) {
        head = tail = new_node;
    } else {
        tail->next = new_node;
        tail = new_node;
    }
    count++;
}

template <typename T, typename Alloc>
void MyContainer<T, Alloc>::clear() {
    Node* current = head;
    while (current) {
        Node* next = current->next;
        alloc.destroy(current);
        alloc.deallocate(current, 1);
        current = next;
    }
    head = tail = nullptr;
    count = 0;
}

template <typename T, typename Alloc>
void MyContainer<T, Alloc>::print() const {
    Node* current = head;
    while (current) {
        std::cout << current->value << " ";
        current = current->next;
    }
    std::cout << std::endl;
}

template <typename T, typename Alloc>
size_t MyContainer<T, Alloc>::size() const {
    return count;
}

template <typename T, typename Alloc>
bool MyContainer<T, Alloc>::empty() const {
    return count == 0;
}

// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
namespace detail {
    inline int factorial(int n) {
        int result = 1;
        for (int i = 2; i <= n; ++i) {
            result *= i;
        }
        return result;
    }
    
    inline void fill_std_map(std::map<int, int>& map) {
        for (int i = 0; i < 10; ++i) {
            map[i] = factorial(i);
        }
    }
    
    inline void fill_custom_map(std::map<int, int, std::less<int>, 
                         allocator<std::pair<const int, int>, 10>>& map) {
        for (int i = 0; i < 10; ++i) {
            map[i] = factorial(i);
        }
    }
    
    inline void fill_my_container(MyContainer<int>& container) {
        for (int i = 0; i < 10; ++i) {
            container.add(i);
        }
    }
    
    inline void fill_my_container_with_alloc(MyContainer<int, allocator<int, 10>>& container) {
        for (int i = 0; i < 10; ++i) {
            container.add(i);
        }
    }
    
    template<typename MapType>
    void print_map(const MapType& map, const std::string& container_name = "") {
        std::cout << (container_name.empty() ? "Map" : container_name) << std::endl;
        std::cout << "Содержимое:" << std::endl;
        
        for (const auto& pair : map) {
            std::cout << "  " << pair.first << " " << pair.second << std::endl;
        }
        
        std::cout << "Размер: " << map.size() << " элементов" << std::endl << std::endl;
    }
    
    template<typename ContainerType>
    void print_container(const ContainerType& container, const std::string& container_name = "") {
        std::cout << (container_name.empty() ? "Container" : container_name) << std::endl;
        std::cout << "Содержимое: ";
        
        container.print();
        
        std::cout << "Размер: " << container.size() << " элементов" << std::endl << std::endl;
    }
}
