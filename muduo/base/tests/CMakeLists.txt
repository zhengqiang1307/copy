add_executable(mutex_test Mutex_test.cpp)
target_link_libraries(mutex_test muduo_base)

add_executable(timestamp_unittest Timestamp_unittest.cpp)
target_link_libraries(timestamp_unittest muduo_base)
add_test(NAME timestamp_unittest COMMAND timestamp_unittest)

add_executable(exception_test Exception_test.cpp)
target_link_libraries(exception_test muduo_base)
add_test(NAME exception_test COMMAND exception_test)

add_executable(thread_test Thread_test.cpp)
target_link_libraries(thread_test muduo_base)
add_test(NAME thread_test COMMAND thread_test)
