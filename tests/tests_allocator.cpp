// Определения для корректной работы Boost.Test
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE AllocatorTests

#include "common.h"
#include <boost/test/unit_test.hpp>

// ============================================
// БАЗОВЫЕ ТЕСТЫ АЛЛОКАТОРА
// ============================================

BOOST_AUTO_TEST_SUITE(BasicAllocatorTests)

BOOST_AUTO_TEST_CASE(AllocateAndConstruct) {
    std::cout << "Тест: Выделение и создание объектов" << std::endl;
    
    allocator<int, 10> alloc;
    
    // Выделяем память
    int* p1 = alloc.allocate(3);
    BOOST_CHECK(p1 != nullptr);
    BOOST_CHECK_EQUAL(alloc.get_used(), 3);
    
    // Создаем объекты
    alloc.construct(p1, 100);
    alloc.construct(p1 + 1, 200);
    alloc.construct(p1 + 2, 300);
    
    BOOST_CHECK_EQUAL(p1[0], 100);
    BOOST_CHECK_EQUAL(p1[1], 200);
    BOOST_CHECK_EQUAL(p1[2], 300);
    
    // Уничтожаем
    alloc.destroy(p1);
    alloc.destroy(p1 + 1);
    alloc.destroy(p1 + 2);
}

BOOST_AUTO_TEST_CASE(AllocatorLimits) {
    std::cout << "Тест: Проверка лимитов аллокатора" << std::endl;
    
    // Тест 1: Запрос больше размера блока вызывает исключение
    {
        allocator<int, 5> alloc;
        BOOST_CHECK_THROW(alloc.allocate(6), std::bad_alloc);
    }
    
    // Тест 2: Заполнение блока до предела работает
    {
        allocator<int, 5> alloc;
        int* p = alloc.allocate(5);
        BOOST_CHECK(p != nullptr);
        BOOST_CHECK_EQUAL(alloc.get_used(), 5);
    }
}

BOOST_AUTO_TEST_SUITE_END()

// ============================================
// ТЕСТЫ С STL КОНТЕЙНЕРАМИ
// ============================================

BOOST_AUTO_TEST_SUITE(STLContainersTests)

BOOST_AUTO_TEST_CASE(StdMapWithAllocator) {
    std::cout << "Тест: std::map с аллокатором" << std::endl;
    
    using MapAlloc = allocator<std::pair<const int, int>, 10>;
    std::map<int, int, std::less<int>, MapAlloc> my_map;
    
    // Заполняем 10 элементами
    for (int i = 0; i < 10; ++i) {
        my_map[i] = i * i;  // квадраты чисел
    }
    
    BOOST_CHECK_EQUAL(my_map.size(), 10);
    BOOST_CHECK_EQUAL(my_map[2], 4);
    BOOST_CHECK_EQUAL(my_map[5], 25);
    BOOST_CHECK_EQUAL(my_map[9], 81);
}

BOOST_AUTO_TEST_CASE(StdVectorWithAllocator) {
    std::cout << "Тест: std::vector с аллокатором" << std::endl;
    
    std::vector<int, allocator<int, 10>> vec;
    
    // Добавляем элементы
    for (int i = 0; i < 5; ++i) {
        vec.push_back(i * 10);
    }
    
    BOOST_CHECK_EQUAL(vec.size(), 5);
    BOOST_CHECK_EQUAL(vec[0], 0);
    BOOST_CHECK_EQUAL(vec[2], 20);
    BOOST_CHECK_EQUAL(vec[4], 40);
}

BOOST_AUTO_TEST_SUITE_END()

// ============================================
// ТЕСТЫ С ПОЛЬЗОВАТЕЛЬСКИМ КОНТЕЙНЕРОМ
// ============================================

BOOST_AUTO_TEST_SUITE(MyContainerTests)

BOOST_AUTO_TEST_CASE(MyContainerBasic) {
    std::cout << "Тест: MyContainer с std::allocator" << std::endl;
    
    MyContainer<int> container;
    
    // Добавляем элементы
    container.add(1);
    container.add(2);
    container.add(3);
    
    BOOST_CHECK_EQUAL(container.size(), 3);
    BOOST_CHECK(!container.empty());
    
    // Очищаем
    container.clear();
    BOOST_CHECK_EQUAL(container.size(), 0);
    BOOST_CHECK(container.empty());
}

BOOST_AUTO_TEST_CASE(MyContainerWithCustomAllocator) {
    std::cout << "Тест: MyContainer с пользовательским аллокатором" << std::endl;
    
    MyContainer<int, allocator<int, 5>> container;
    
    // Добавляем элементы
    container.add(10);
    container.add(20);
    container.add(30);
    
    BOOST_CHECK_EQUAL(container.size(), 3);
}

BOOST_AUTO_TEST_SUITE_END()

// ============================================
// ДЕМОНСТРАЦИОННЫЙ ТЕСТ (ОСНОВНОЕ ЗАДАНИЕ)
// ============================================

BOOST_AUTO_TEST_CASE(DemoTestFromAssignment) {
    std::cout << "\n=== ДЕМОНСТРАЦИОННЫЙ ТЕСТ ИЗ ЗАДАНИЯ ===" << std::endl;
    
    // 1. Функция факториала
    auto factorial = [](int n) -> int {
        int result = 1;
        for (int i = 2; i <= n; ++i) result *= i;
        return result;
    };
    
    // 2. std::map с нашим аллокатором
    std::cout << "1. Создание std::map с аллокатором (10 элементов):" << std::endl;
    using MapAlloc = allocator<std::pair<const int, int>, 10>;
    std::map<int, int, std::less<int>, MapAlloc> fact_map;
    
    for (int i = 0; i < 10; ++i) {
        fact_map[i] = factorial(i);
    }
    
    // Проверка
    BOOST_CHECK_EQUAL(fact_map.size(), 10);
    BOOST_CHECK_EQUAL(fact_map[0], 1);
    BOOST_CHECK_EQUAL(fact_map[1], 1);
    BOOST_CHECK_EQUAL(fact_map[3], 6);
    BOOST_CHECK_EQUAL(fact_map[5], 120);
    
    // 3. Наш контейнер с нашим аллокатором
    std::cout << "2. Создание MyContainer с аллокатором (10 элементов):" << std::endl;
    MyContainer<int, allocator<int, 10>> container;
    
    for (int i = 0; i < 10; ++i) {
        container.add(i);
    }
    
    // Проверка
    BOOST_CHECK_EQUAL(container.size(), 10);
    BOOST_CHECK(!container.empty());
    
    std::cout << "=== ТЕСТ ЗАВЕРШЕН ===" << std::endl;
}

// ============================================
// ДОПОЛНИТЕЛЬНЫЕ ТЕСТЫ
// ============================================

BOOST_AUTO_TEST_CASE(AllocatorEquality) {
    std::cout << "Тест: Сравнение аллокаторов" << std::endl;
    
    allocator<int, 10> alloc1;
    allocator<double, 10> alloc2;
    
    // Аллокаторы с одинаковым размером считаются равными
    BOOST_CHECK(alloc1 == alloc2);
    BOOST_CHECK(!(alloc1 != alloc2));
}

BOOST_AUTO_TEST_CASE(StringAllocation) {
    std::cout << "Тест: Работа со строками" << std::endl;
    
    allocator<std::string, 3> alloc;
    
    // Выделяем и создаем строки
    std::string* s1 = alloc.allocate(1);
    alloc.construct(s1, "Hello");
    
    std::string* s2 = alloc.allocate(1);
    alloc.construct(s2, "World");
    
    BOOST_CHECK_EQUAL(*s1, "Hello");
    BOOST_CHECK_EQUAL(*s2, "World");
    
    alloc.destroy(s1);
    alloc.destroy(s2);
}