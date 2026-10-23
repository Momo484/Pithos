cmake_minimum_required(VERSION 3.14)
project(PithosBackend VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_AUTOMOC ON)                   # required for Qt's moc preprocessor
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # for clangd

# Default to Release build type if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the build type: Debug Release RelWithDebInfo MinSizeRel" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

# Apply aggressive optimizations for release builds (gives ~3.5x NPS improvement)
if(CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo")
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
    add_compile_options(-O3 -march=native -DNDEBUG)
    add_link_options(-flto)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-O3 -march=native -DNDEBUG)
    add_link_options(-flto)
  else()
    # Fallback for other compilers
    add_compile_options(-O3 -DNDEBUG)
  endif()
endif()

# --- 1. Fetch Catch2 from GitHub ---
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.5.2
)
FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

# --- 2. Find Qt6 ---
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# --- 3. Gather Source Files ---
file(GLOB_RECURSE CORE_SOURCES "src/backend/core/*.cpp")
file(GLOB_RECURSE TEST_SOURCES "src/backend/testing/*.cpp")
file(GLOB_RECURSE FRONTEND_SOURCES "src/frontend/*.cpp")
file(GLOB_RECURSE HERODOTUS_SOURCES "src/engines/herodotus/*.cpp")
file(GLOB_RECURSE HERODOTUS_TEST_SOURCES "src/engines/herodotus_testing/*.cpp")
qt6_add_resources(RESOURCES src/frontend/resources/resources.qrc)

# --- 4. Build Core Library ---
add_library(PithosCore ${CORE_SOURCES})
target_include_directories(PithosCore PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/backend/core
)

# --- 5. Build Main Engine ---
add_executable(pithos_backend src/backend/main.cpp)
target_link_libraries(pithos_backend PRIVATE PithosCore)
target_include_directories(pithos_backend PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/backend/core
)

# --- 6. Build Testing Suite ---
add_executable(backend_tests ${TEST_SOURCES})
target_link_libraries(backend_tests PRIVATE PithosCore Catch2::Catch2WithMain)
target_include_directories(backend_tests PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/backend/core
)

# --- 7. Build Frontend ---
add_executable(pithos_chess ${FRONTEND_SOURCES} ${RESOURCES})
target_link_libraries(pithos_chess PRIVATE PithosCore Qt6::Widgets)
target_include_directories(pithos_chess PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/backend/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/frontend
)

# --- 8a. Build Herodotus Core Library ---
# 1. Grab all engine sources
file(GLOB_RECURSE HERODOTUS_CORE_SOURCES "src/engines/herodotus/*.cpp")

# 2. IMPORTANT: Remove main.cpp so it doesn't conflict with Catch2!
# Adjust this path if your main.cpp is named or located differently.
list(REMOVE_ITEM HERODOTUS_CORE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/engines/herodotus/main.cpp")

# The magic generator is a standalone tool with its own main(); keep it out of
# the library for the same reason as main.cpp.
list(REMOVE_ITEM HERODOTUS_CORE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/engines/herodotus/utils/generate_magics.cpp")

# 3. Create the library
add_library(HerodotusCore ${HERODOTUS_CORE_SOURCES})
target_include_directories(HerodotusCore PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/engines/herodotus
)
# Assuming Herodotus needs PithosCore for basic board representations
target_link_libraries(HerodotusCore PRIVATE PithosCore) 


# --- 8b. Build Herodotus Main Executable ---
# Compile ONLY the main file, and link it to the core library
add_executable(herodotus src/engines/herodotus/main.cpp)
target_link_libraries(herodotus PRIVATE HerodotusCore)


# --- 9. Build Herodotus Engine Tests ---
add_executable(herodotus_test ${HERODOTUS_TEST_SOURCES})

# Now we link HerodotusCore so the tests have access to the engine implementation!
target_link_libraries(herodotus_test PRIVATE HerodotusCore Catch2::Catch2WithMain)


# --- 10. Magic number generator (standalone tool) ---
# Self-contained: compiles MagicBitboards.cpp directly so it builds even while
# other parts of the engine are mid-refactor.
add_executable(generate_magics
    src/engines/herodotus/utils/generate_magics.cpp
    src/engines/herodotus/utils/MagicBitboards.cpp
)
target_include_directories(generate_magics PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/engines/herodotus
)
