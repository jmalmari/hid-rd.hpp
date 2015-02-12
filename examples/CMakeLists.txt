set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${HIDRD_SOURCE_DIR}/include)

add_executable(dump dump.cpp)
