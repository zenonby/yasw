cmake_minimum_required(VERSION 3.5)

# Use _ROOT variables to find packages
cmake_policy(SET CMP0074 NEW)
cmake_policy(SET CMP0091 NEW)

# Statically link MS VC++ runtime library
if(MSVC)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

project(yasw
    VERSION   0.1
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_SOURCES
	amalgamation/sqlite3.c
	amalgamation/sqlite3.h
	src/SqliteDb.cpp
	include/${PROJECT_NAME}/SqliteDb.h
	src/SqliteRecordset.cpp
	include/${PROJECT_NAME}/SqliteRecordset.h
	src/SqliteCommand.cpp
	include/${PROJECT_NAME}/SqliteCommand.h
	src/SqliteTransaction.cpp
	include/${PROJECT_NAME}/SqliteTransaction.h
)

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME   ${PROJECT_NAME}
    PUBLIC_HEADER SqliteDb.h
	ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
    DEBUG_POSTFIX d
)

target_include_directories(${PROJECT_NAME} PRIVATE include/${PROJECT_NAME} amalgamation)
target_include_directories(${PROJECT_NAME} PUBLIC include)

### Tests with Boost.Test

set(TESTS_SOURCES
	tests/TestSqliteDb.cpp
	tests/TestSqliteDbBindings.cpp)

# Find boost
find_package(BOOST REQUIRED COMPONENTS unit_test_framework)
include_directories(${Boost_INCLUDE_DIRS})

set(TEST test_${PROJECT_NAME})
add_executable(${TEST} ${TESTS_SOURCES})

target_include_directories(${TEST} PRIVATE include/${PROJECT_NAME})
target_link_libraries(${TEST} ${PROJECT} ${PROJECT_NAME} ${Boost_LIBRARIES})

enable_testing()
add_test(${TEST} ${TEST})
