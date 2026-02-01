#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <memory>
#include <cstddef>
#include <type_traits>
#include <typeinfo>
#include "common.h"

int main() {

    using namespace detail;
    
    // 1. Создание и заполнение std::map<int, int>
    std::map<int, int> standard_map;
    fill_std_map(standard_map);
    print_map(standard_map, "std::map (стандартный аллокатор)");

    // 2. Создание и заполнение std::map с нашим аллокатором
    using CustomMapAlloc = allocator<std::pair<const int, int>, 10>;
    std::map<int, int, std::less<int>, CustomMapAlloc> custom_map;
    fill_custom_map(custom_map);
    print_map(custom_map, "std::map (наш аллокатор, 10 элементов)");
    
    // 4. Создание и заполнение нашего контейнера
    MyContainer<int> my_container;
    fill_my_container(my_container);
    print_container(my_container, "MyContainer (стандартный аллокатор)");
    
    // 5. Создание и заполнение нашего контейнера с нашим аллокатором
    MyContainer<int, allocator<int, 10>> my_container_with_alloc;
    fill_my_container_with_alloc(my_container_with_alloc);
    print_container(my_container_with_alloc, "MyContainer (наш аллокатор, 10 элементов)");

}