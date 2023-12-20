cmake_minimum_required(VERSION 3.0.0)
project(SentinelChess VERSION 0.1.0 LANGUAGES C CXX)

file(GLOB SRCS *.cpp *.h)
add_executable(SentinelChess ${SRCS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
