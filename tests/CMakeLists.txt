set(Boost_NO_BOOST_CMAKE ON)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

add_executable(allocator_tests tests_allocator.cpp)

target_include_directories(allocator_tests PRIVATE ${PROJECT_SOURCE_DIR}/src/include ${Boost_INCLUDE_DIRS})

target_link_libraries(allocator_tests PRIVATE ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(NAME allocator_tests COMMAND allocator_tests)

