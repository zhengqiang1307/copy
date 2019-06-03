add_executable(mutex_test Mutex_test.cpp)
target_link_libraries(mutex_test muduo_base)
add_test(NAME mutex_test COMMAND mutex_test)

add_executable(timestamp_unittest Timestamp_unittest.cpp)
target_link_libraries(timestamp_unittest muduo_base)
add_test(NAME timestamp_unittest COMMAND timestamp_unittest)

add_executable(exception_test Exception_test.cpp)
target_link_libraries(exception_test muduo_base)
add_test(NAME exception_test COMMAND exception_test)

add_executable(thread_test Thread_test.cpp)
target_link_libraries(thread_test muduo_base)
add_test(NAME thread_test COMMAND thread_test)

add_executable(Singleton_test Singleton_test.cpp)
target_link_libraries(Singleton_test muduo_base)
add_test(NAME Singleton_test COMMAND Singleton_test)

#if(BOOSTTEST_LIBRARY)
add_executable(LogStream_test LogStream_test.cpp)
target_link_libraries(LogStream_test muduo_base boost_unit_test_framework)
add_test(NAME LogStream_test COMMAND LogStream_test)
#endif()

add_executable(TimeZone_test TimeZone_test.cpp)
target_link_libraries(TimeZone_test muduo_base)
add_test(NAME TimeZone_test COMMAND TimeZone_testy)

add_executable(Date_test Date_test.cpp)
target_link_libraries(Date_test muduo_base)
add_test(NAME Date_test COMMAND Date_test)

add_executable(ThreadLocal_test ThreadLocal_test.cpp)
target_link_libraries(ThreadLocal_test muduo_base)
add_test(NAME ThreadLocal_test COMMAND ThreadLocal_test)

add_executable(ThreadLocalSingleton_test ThreadLocalSingleton_test.cpp)
target_link_libraries(ThreadLocalSingleton_test muduo_base)
add_test(NAME ThreadLocalSingleton_test COMMAND ThreadLocalSingleton_test)

add_executable(ThreadPool_test ThreadPool_test.cpp)
target_link_libraries(ThreadPool_test muduo_base)
add_test(NAME ThreadPool_test COMMAND ThreadPool_test)

add_executable(ProcessInfoTest ProcessInfoTest.cpp)
target_link_libraries(ProcessInfoTest muduo_base)
add_test(NAME ProcessInfoTest COMMAND ProcessInfoTest)

add_executable(LogStreamBenchTest LogStreamBenchTest.cpp)
target_link_libraries(LogStreamBenchTest muduo_base)
add_test(NAME LogStreamBenchTest COMMAND LogStreamBenchTest)

add_executable(LoggingTest LoggingTest.cpp)
target_link_libraries(LoggingTest muduo_base)
add_test(NAME LoggingTest COMMAND LoggingTest)

add_executable(FileUtilTest FileUtilTest.cpp)
target_link_libraries(FileUtilTest muduo_base)
add_test(NAME FileUtilTest COMMAND FileUtilTest)
