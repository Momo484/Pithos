cmake_minimum_required(VERSION 3.14)
project(PithosBackend VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_AUTOMOC ON)                   # required for Qt's moc preprocessor
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # for clangd

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

# --- 8. Build Herodotus Engine ---
add_executable(herodotus ${HERODOTUS_SOURCES})

target_include_directories(herodotus PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/engines/herodotus
)
