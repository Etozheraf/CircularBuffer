add_library(CCircularBuffer SHARED CCircularBuffer.cpp)

target_include_directories(CCircularBuffer INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})