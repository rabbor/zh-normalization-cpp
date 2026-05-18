cmake_minimum_required(VERSION 3.15)
project(zh-normalize LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/third/sparrowhawk-install/include)
include_directories(${CMAKE_SOURCE_DIR}/third/abseil-install/include)
include_directories(${CMAKE_SOURCE_DIR}/third/openfst-1.6.7-install/include)
include_directories(${CMAKE_SOURCE_DIR}/third/protobuf-25-install/include)
include_directories(${CMAKE_SOURCE_DIR}/third/re2-install/include)
include_directories(${CMAKE_SOURCE_DIR}/third/thrax-1.2.8-install/include)

link_directories(${CMAKE_SOURCE_DIR}/third/sparrowhawk-install/lib)
link_directories(${CMAKE_SOURCE_DIR}/third/abseil-install/lib)
link_directories(${CMAKE_SOURCE_DIR}/third/openfst-1.6.7-install/lib)
link_directories(${CMAKE_SOURCE_DIR}/third/protobuf-25-install/lib)
link_directories(${CMAKE_SOURCE_DIR}/third/re2-install/lib)
link_directories(${CMAKE_SOURCE_DIR}/third/thrax-1.2.8-install/lib)

# 运行时 rpath：告知 dyld 去这些目录找 @rpath/libxxx.dylib
set(CMAKE_BUILD_RPATH
        ${CMAKE_SOURCE_DIR}/third/sparrowhawk-install/lib
        ${CMAKE_SOURCE_DIR}/third/openfst-1.6.7-install/lib
        ${CMAKE_SOURCE_DIR}/third/protobuf-25-install/lib
        ${CMAKE_SOURCE_DIR}/third/re2-install/lib
        ${CMAKE_SOURCE_DIR}/third/thrax-1.2.8-install/lib
        ${CMAKE_SOURCE_DIR}/third/abseil-install/lib
)

include_directories(${CMAKE_SOURCE_DIR}/src)
add_executable(zh-normalize ${CMAKE_SOURCE_DIR}/src/main.cpp
        ${CMAKE_SOURCE_DIR}/src/tts_robust_normalizer.cpp)
target_link_libraries(zh-normalize sparrowhawk fst thrax re2 protobuf)

## test
add_executable(test_tts_robust_normalizer
        ${CMAKE_SOURCE_DIR}/src/test_tts_robust_normalizer.cpp
        ${CMAKE_SOURCE_DIR}/src/tts_robust_normalizer.cpp)
target_link_libraries(test_tts_robust_normalizer sparrowhawk fst thrax re2 protobuf)