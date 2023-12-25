cmake_minimum_required(VERSION 3.24)
project(untitled)

set(CMAKE_CXX_STANDARD 17)


add_executable(         ${PROJECT_NAME} main.cpp
                        ${PROJECT_SOURCE_DIR}/parser/parser.cpp
                        ${PROJECT_SOURCE_DIR}/parser/parser.h
                        ${PROJECT_SOURCE_DIR}/write/write.cpp
                        ${PROJECT_SOURCE_DIR}/write/write.h
                        ${PROJECT_SOURCE_DIR}/hamming/hamming.cpp
                        ${PROJECT_SOURCE_DIR}/hamming/hamming.h)

include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/parser
                                                ${PROJECT_SOURCE_DIR}/write
                                                ${PROJECT_SOURCE_DIR}/hamming)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_subdirectory(tests)
