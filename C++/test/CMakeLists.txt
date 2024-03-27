#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON)

file(GLOB TestSources *.cpp)

include_directories("../googletest/googletest/include")
add_executable(${SC_TEST_EXE} ${TestSources})

target_link_libraries(${SC_TEST_EXE} PUBLIC SentinelLib)
target_link_libraries(${SC_TEST_EXE} PUBLIC gtest gmock_main)

include(GoogleTest)
gtest_discover_tests(${SC_TEST_EXE})