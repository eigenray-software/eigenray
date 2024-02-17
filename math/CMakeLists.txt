
project(eigenray_math)

# file(TOUCH ${CMAKE_BINARY_DIR}/stub.cpp)

add_library(eigenray_math INTERFACE)
target_include_directories(eigenray_math INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(eigenray_math PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/vec.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/defines.hpp
)

target_include_directories(eigenray_math INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)

set_target_properties(eigenray_math PROPERTIES PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/defines.hpp;${CMAKE_CURRENT_SOURCE_DIR}/vec.hpp")

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
  if(WIN32)
    set(CMAKE_CXX_STANDARD 20)
    add_executable(test ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp)
    target_link_libraries(test PUBLIC vkl)
  endif()
endif()


# add_custom_target(eigenray_math_ SOURCES 
#     ${CMAKE_CURRENT_SOURCE_DIR}/vec.hpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/defines.hpp
#     )

