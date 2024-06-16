cmake_minimum_required(VERSION 3.20)
project(crashlog-decrypter)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
    main.cpp
)

add_executable(crashlog-decrypter ${SOURCES})
