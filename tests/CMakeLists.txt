set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

include_directories(${HIDRD_SOURCE_DIR}/include)

macro(addExeTest tname)
  add_executable(test-${tname} ${ARGN})
  add_test(${tname} test-${tname})
  add_dependencies(check test-${tname})
endmacro()

addExeTest(bytes bytes.cpp)
addExeTest(typechain typechain.cpp)
addExeTest(itemsize itemsize.cpp)
addExeTest(descriptor descriptor.cpp)
