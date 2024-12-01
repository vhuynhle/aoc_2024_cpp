# Part 1
add_executable(distance_test)
target_sources(distance_test PRIVATE distance.cpp distance_test.cpp)
target_link_libraries(distance_test gtest gtest_main)

add_executable(distance)
target_sources(distance PRIVATE distance.cpp distance_main.cpp)


# Part 2
add_executable(similarity_test)
target_sources(similarity_test PRIVATE similarity.cpp similarity_test.cpp)
target_link_libraries(similarity_test gtest gtest_main)

add_executable(similarity)
target_sources(similarity PRIVATE similarity.cpp similarity_main.cpp)
