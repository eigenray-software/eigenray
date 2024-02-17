
project(eigenray_vk)

file(GLOB eigenray_vk_sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(eigenray_vk STATIC ${eigenray_vk_sources})
target_link_libraries(eigenray_vk PUBLIC eigenray_vkl eigenray_math)



target_include_directories(eigenray_vk
                           PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                            $<INSTALL_INTERFACE:include>
                           )
