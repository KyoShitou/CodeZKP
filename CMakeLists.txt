# CMake minimum version required.
# FetchContent needs >= 3.11, Google Test requires C++14 support.
cmake_minimum_required(VERSION 3.14)

# Define the project name and specify C++ as the language.
project(FieldAndPolyTesting LANGUAGES CXX)

# Set the C++ standard. Google Test requires at least C++14.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Build Type Configuration (for CLion/Debug builds) ---
# Ensure a default build type (like Debug) is set if none is specified externally
# (e.g., by CLion profiles or -DCMAKE_BUILD_TYPE). Debug is needed for GDB/LLDB.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type (Debug, Release, ...)" FORCE)
  message(STATUS "Build type not specified, defaulting to: ${CMAKE_BUILD_TYPE}")
endif()
message(STATUS "Using build type: ${CMAKE_BUILD_TYPE}")


# --- Dependency Setup ---

# Define the path to the ttmath header directory.
# Assumes ttmath headers are in '/src/ttmath/' relative to this CMakeLists.txt.
set(TTMATH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/ttmath)
# Verify the main ttmath header exists at the specified path.
if(NOT EXISTS "${TTMATH_INCLUDE_DIR}/ttmath.h")
    message(FATAL_ERROR "ttmath headers not found in ${TTMATH_INCLUDE_DIR}. Please check the path.")
endif()
message(STATUS "Using ttmath headers from: ${TTMATH_INCLUDE_DIR}")

# --- Google Test Setup ---
# Use FetchContent to automatically download and configure Google Test.
# This avoids needing a system-wide installation.
include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the Git repository and a specific tag for reproducibility.
  # Using a tag (like release-1.14.0) is generally recommended over 'main'.
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        main
  # Alternatively, use a direct URL to a zip archive:
  # URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# For Windows compatibility: Ensure Google Test uses the same CRT linkage as the project.
if(MSVC)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

# Download Google Test source and make its targets (like GTest::gtest_main) available.
FetchContent_MakeAvailable(googletest)


# --- Testing Setup ---
# Enable integration with CTest (CMake's testing framework).
enable_testing()


# --- Test Executable Definition ---
# Define the executable target that will contain your tests.
# Source files are specified relative to the location of this CMakeLists.txt.
add_executable(
  field_poly_tests         # The name of the test executable binary
  test/polynomials_test.cpp         # The source file containing all tests
)


# --- Include Directories Configuration ---
# Specify directories the compiler should search for header files (#include <...>)
# for the 'field_poly_tests' target.
target_include_directories(
  field_poly_tests         # The target executable
  PRIVATE
    # Add the '/src' directory. This allows finding:
    # - "field.hpp" (directly in src)
    # - "ttmath/ttmath.h" (subdirectory within src)
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    # No need to add TTMATH_INCLUDE_DIR separately as it's covered by '/src'
)


# --- Linking Dependencies ---
# Link the test executable against the necessary libraries.
target_link_libraries(
  field_poly_tests         # The target executable
  PRIVATE
    # Link against GTest::gtest_main. This target includes:
    # - The Google Test library itself (GTest::gtest)
    # - A pre-defined main() function that initializes and runs all tests
    # - Necessary system libraries (like pthreads)
    GTest::gtest_main
    # ttmath is header-only, so no library needs to be linked here.
    # GMPXX::gmpxx is removed as GMP is no longer used.
)


# --- Test Discovery for CTest/CLion ---
# Include the GoogleTest module to enable automatic test discovery.
# This allows CTest and CLion's test runner to find individual tests
# defined with TEST(), TEST_F(), etc., within the executable.
include(GoogleTest)
gtest_discover_tests(
  field_poly_tests         # The target executable whose tests should be discovered
)


# --- Final Informational Messages ---
message(STATUS "CMake configuration successful for project: ${PROJECT_NAME}")
message(STATUS "Test executable target created: field_poly_tests")
message(STATUS "Build using CLion or 'cmake --build <build_dir>'.")
message(STATUS "Run tests via CLion's UI or 'ctest' in the build directory.")

