# Minimum CMake version required (FetchContent needs >= 3.11, GTest needs C++14 support)
cmake_minimum_required(VERSION 3.14)

# Project Name and Language
project(FieldLibProject LANGUAGES CXX)

# Set C++ standard (Google Test requires at least C++14)
# Using C++11 as specified in the previous test code might work for that specific
# code, but GTest itself generally expects C++14 or later.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Optional: Disable compiler-specific extensions for better portability
# set(CMAKE_CXX_EXTENSIONS OFF)

# --- Google Test ---
# Use FetchContent to download and configure Google Test during the build configuration.
# This avoids needing to install Google Test system-wide.
include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the repository and a specific version (tag or commit hash)
  # Using a specific tag/hash ensures reproducibility. Check for newer tags.
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  # You can add URL_HASH SHA256=... here for an integrity check if desired
)

# --- Find Dependencies ---

# Define the path to ttmath headers (User specified: src/ttmath)
set(TTMATH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/ttmath) # <-- Updated path
if(NOT EXISTS "${TTMATH_INCLUDE_DIR}/ttmath.h")
    message(FATAL_ERROR "ttmath headers not found in ${TTMATH_INCLUDE_DIR}. Please check the path.")
endif()
message(STATUS "Using ttmath headers from: ${TTMATH_INCLUDE_DIR}")

# For Windows: Set gtest_force_shared_crt ON to avoid potential runtime conflicts.
if(MSVC)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

# Make Google Test available (downloads and adds its targets like GTest::gtest_main)
# The BUILD_GMOCK=OFF option prevents building Google Mock if you don't need it.
# The INSTALL_GTEST=OFF option prevents installing Google Test during 'make install'.
FetchContent_MakeAvailable(googletest)

# --- Testing Setup ---
# Enable CTest, the CMake test driver program.
# This allows you to run tests using 'ctest' command after building.
enable_testing()

# --- Test Executable ---
# Define the executable target for your tests.
# CMake automatically knows to look for test/fieldtest.cpp relative to this CMakeLists.txt
add_executable(
  field_test_large_prime_runner # The name of the test executable binary
  test/fieldtest_large_prime.cpp # The source file containing your tests
)

# --- Include Directories ---
# Add include directories needed by the test runner:
# - Project root (to find Field.hpp)
# - GMP include directory
# - ttmath include directory (via ttmath target)
target_include_directories(
  field_test_large_prime_runner
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR} # For Field.hpp
    ${TTMATH_INCLUDE_DIR}           # ttmath includes are handled by linking the ttmath INTERFACE library below
)

# --- Link Dependencies ---
# Link the test executable against Google Test.
# GTest::gtest_main provides a default main() function and links GTest::gtest.
# It automatically handles platform-specific dependencies like pthreads.
target_link_libraries(
  field_test_large_prime_runner # The target executable
  PRIVATE
    GTest::gtest_main # Google Test
)

# --- Discover Tests for CTest ---
# Include the GoogleTest module provided by CMake (requires CMake >= 3.10)
# This allows CTest to automatically discover the tests defined in your executable.
include(GoogleTest)
gtest_discover_tests(
  field_test_large_prime_runner # The target whose tests should be discovered
)

# --- Optional: Instructions if your tests NEEDED Field.hpp ---
# If your 'fieldtest.cpp' were modified to #include "Field.hpp"
# and use the *actual* FieldElement class, you would need to:
#
# 1. Find GMP and ttmath (assuming ttmath headers are in 'libs/ttmath/ttmath'):
#    find_package(GMPXX REQUIRED)
#    add_library(ttmath INTERFACE)
#    target_include_directories(ttmath INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/libs/ttmath)
#
# 2. Add the project root to the include path for the test runner:
#    target_include_directories(field_test_runner PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
#
# 3. Link GMP and ttmath to the test runner:
#    target_link_libraries(field_test_runner PRIVATE GMPXX::gmpxx ttmath)
#
# --- End Optional Section ---

# Informative message (optional)
message(STATUS "Google Test configured for target: field_test_large_prime_runner")