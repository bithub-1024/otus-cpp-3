add_executable(allocator allocator.cpp)

target_include_directories(allocator PRIVATE 
    ${PROJECT_SOURCE_DIR}/
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/include
)

install(TARGETS allocator RUNTIME DESTINATION bin)
