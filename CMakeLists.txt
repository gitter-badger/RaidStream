cmake_minimum_required(VERSION 3.16)
project(raidstream
    LANGUAGES CXX
)
set(CMAKE_VERBOSE_MAKEFILE True)
set(CMAKE_CXX_STANDARD 17)
set(RAIDSTREAM_SOURCES
        src/RaidStream/crc32/crc32.cpp
        src/RaidStream/crc32/crc32.hpp
        src/RaidStream/crc64/crc64.cpp
        src/RaidStream/RaidStream.hpp
        src/RaidStream/RaidConfiguration.hpp
        src/RaidStream/RaidFile.hpp
        src/RaidStream/RaidFileBlock.hpp
        src/RaidStream/RaidFileStripe.hpp
        src/RaidStream/test.cpp
)
add_library(raidstream-object OBJECT ${RAIDSTREAM_SOURCES})
add_library(raidstream-static STATIC $<TARGET_OBJECTS:raidstream-object>)
add_library(raidstream-shared SHARED $<TARGET_OBJECTS:raidstream-object>)
target_include_directories(raidstream-object SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(raidstream-static SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(raidstream-shared SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Assume platform is linux until proven otherwise.
set(CMAKE_PLATFORM_TEST "IS_LINUX")
#if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
#    set(CMAKE_PLATFORM_TEST "IS_LINUX")
#endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CMAKE_PLATFORM_TEST "IS_MACOS")
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_PLATFORM_TEST "IS_WINDOWS")
endif()
target_compile_definitions(raidstream-object PUBLIC "${CMAKE_PLATFORM_TEST}")
target_compile_definitions(raidstream-static PUBLIC "${CMAKE_PLATFORM_TEST}")
target_compile_definitions(raidstream-shared PUBLIC "${CMAKE_PLATFORM_TEST}")

add_executable(raidstream-test src/RaidStream/test.cpp)
target_link_libraries(raidstream-test raidstream-static)
target_include_directories(raidstream-test SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
