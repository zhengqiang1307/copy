include(CheckFunctionExists)

check_function_exists(accept4 HAVE_ACCEPT4)
if(NOT HAVE_ACCEPT4)
  set_source_files_properties(SocketsOps.cpp PROPERTIES COMPILE_FLAGS "-DNO_ACCEPT4")
endif()

#set(base_SRCS
#    CurrentThread.cc
#)
file(GLOB net_SRCS "*.cpp")
add_library(muduo_net ${net_SRCS})
target_link_libraries(muduo_net muduo_base)
set_target_properties(muduo_net PROPERTIES COMPILE_FLAGS "-std=c++0x")

install(TARGETS muduo_net DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/muduo/net)

#add_subdirectory(http)
#add_subdirectory(inspect)

#if(MUDUO_BUILD_EXAMPLES)
#  add_subdirectory(tests)
#endif()

#if(PROTOBUF_FOUND)
#  add_subdirectory(protobuf)
#  add_subdirectory(protorpc)
#else()
#  add_subdirectory(protobuf EXCLUDE_FROM_ALL)
#  add_subdirectory(protorpc EXCLUDE_FROM_ALL)
#endif()
