#set(base_SRCS
#    CurrentThread.cc
#)
file(GLOB base_SRCS "*.cpp")
add_library(muduo_base ${base_SRCS})
target_link_libraries(muduo_base pthread rt)
set_target_properties(muduo_base PROPERTIES COMPILE_FLAGS "-std=c++0x")

install(TARGETS muduo_base DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/muduo/base)

if(MUDUO_BUILD_EXAMPLES)
  add_subdirectory(tests)
endif()
