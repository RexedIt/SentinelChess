file(GLOB ConsoleSources *.cpp)
add_executable(SentinelConsole ${ConsoleSources})
target_link_libraries(SentinelConsole PUBLIC SentinelLib)

