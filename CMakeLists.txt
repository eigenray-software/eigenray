cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 20)


project(eigenray)

add_subdirectory(vkl)
add_subdirectory(vk)
add_subdirectory(math)

add_executable(eigenray main.cpp)
target_link_libraries(eigenray eigenray_vk eigenray_math)