set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

include_directories(${HIDRD_SOURCE_DIR}/include)

macro(addExeTest tname)
  add_executable(${tname}-testapp EXCLUDE_FROM_ALL ${ARGN})
  add_test(${tname} ${tname}-testapp)
  add_dependencies(check ${tname}-testapp)
endmacro()

addExeTest(bytes bytes.cpp)
addExeTest(typechain typechain.cpp)
addExeTest(descriptor descriptor.cpp)
