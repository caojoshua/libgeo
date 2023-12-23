add_executable(
  datastruct_test
  priority_queue.cpp
  red_black_tree.cpp
  vector.cpp
  )

target_link_libraries(
  datastruct_test
  PUBLIC geodatastruct
  GTest::gtest_main
  )

gtest_discover_tests(datastruct_test)
