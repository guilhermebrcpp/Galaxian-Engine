cmake_minimum_required(VERSION 3.23) 

project(Galaxian_Engine) 

file(GLOB SOURCES "Source/*.cpp") 

add_compile_options(-O2 -mavx2 -mfma) 

add_executable(${PROJECT_NAME} ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/Header"
)

target_link_options(${PROJECT_NAME} PRIVATE
    -static-libgcc
    -static-libstdc++
)