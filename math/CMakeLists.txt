
project(eigenray_math)

# file(TOUCH ${CMAKE_BINARY_DIR}/stub.cpp)

add_library(eigenray_math INTERFACE)

target_sources(eigenray_math PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/vec.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/defines.hpp
)

target_include_directories(eigenray_math INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)

set_target_properties(eigenray_math PROPERTIES PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/defines.hpp;${CMAKE_CURRENT_SOURCE_DIR}/vec.hpp")

