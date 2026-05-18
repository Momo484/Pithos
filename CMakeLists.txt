cmake_minimum_required(VERSION 3.14) # FetchContent works best on 3.14+

project(PithosBackend VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# --- 1. Fetch Catch2 from GitHub ---
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.5.2 # The specific version to download
)
FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

# --- 2. Gather Source Files ---
file(GLOB_RECURSE CORE_SOURCES "src/backend/core/*.cpp")
file(GLOB_RECURSE TEST_SOURCES "src/backend/testing/*.cpp")

# --- 3. Build Core Library ---
add_library(PithosCore ${CORE_SOURCES})
target_include_directories(PithosCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/backend/core)

# --- 4. Build Main Engine ---
add_executable(pithos_backend src/backend/main.cpp)
target_link_libraries(pithos_backend PRIVATE PithosCore)
target_include_directories(pithos_backend PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/backend/core)

# --- 5. Build Testing Suite ---
add_executable(backend_tests ${TEST_SOURCES})
# Link your core logic, AND link the Catch2 framework we just downloaded
target_link_libraries(backend_tests PRIVATE PithosCore Catch2::Catch2WithMain)
target_include_directories(backend_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/backend/core)