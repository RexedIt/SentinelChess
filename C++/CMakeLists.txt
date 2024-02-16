cmake_minimum_required(VERSION 3.0.0)
project(SentinelChess VERSION 0.2.0 LANGUAGES C CXX)

if (MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# build the console app and include dependencies
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Console)

include_directories("SentinelLib")
add_subdirectory("SentinelLib")
add_subdirectory("SentinelConsole")

file(COPY SentinelConsole/readme.txt DESTINATION ${CMAKE_BINARY_DIR}/Console/Debug)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)