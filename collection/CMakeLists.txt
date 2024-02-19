
project(eigenray_collection)

# file(TOUCH ${CMAKE_BINARY_DIR}/stub.cpp)

add_library(eigenray_collection INTERFACE)

target_include_directories(eigenray_collection INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)



