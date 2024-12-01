add_executable(distance_test)
target_sources(distance_test PRIVATE distance.cpp distance_test.cpp)
target_link_libraries(distance_test gtest gtest_main)

add_executable(distance)
target_sources(distance PRIVATE distance.cpp distance_main.cpp)
