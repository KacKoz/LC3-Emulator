cmake_minimum_required(VERSION 3.10)
project(LC3VM)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(lc3-vm 
    main.cpp 
    lc3vm/vm.cpp
    lc3vm/cpu.cpp
    lc3vm/memory_manager.cpp
)
