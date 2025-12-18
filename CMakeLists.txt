cmake_minimum_required(VERSION 3.20)
project(lv VERSION 0.1.0 LANGUAGES C CXX)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(LV_CPP_USE_STD_FUNCTION "Enable std::function callbacks (heap allocation)" OFF)
option(LV_BUILD_EXAMPLES "Build examples" ON)
option(LV_BUILD_DEMOS "Build demos" ON)
option(LV_BUILD_TESTS "Build tests" OFF)

# Find LVGL - expect it to be a sibling directory or provided via LVGL_DIR
if(NOT TARGET lvgl)
    if(DEFINED LVGL_DIR)
        add_subdirectory(${LVGL_DIR} ${CMAKE_BINARY_DIR}/lvgl)
    elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../lvgl/CMakeLists.txt)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../lvgl ${CMAKE_BINARY_DIR}/lvgl)
    else()
        message(FATAL_ERROR "LVGL not found. Set LVGL_DIR or place lvgl as sibling directory.")
    endif()
endif()

# Header-only library
add_library(lv INTERFACE)
add_library(lv::lv ALIAS lv)

target_include_directories(lv INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(lv INTERFACE lvgl)

target_compile_features(lv INTERFACE cxx_std_20)

# Configuration options as compile definitions
if(LV_CPP_USE_STD_FUNCTION)
    target_compile_definitions(lv INTERFACE LV_CPP_USE_STD_FUNCTION=1)
endif()

# Compiler warnings for development
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(lv INTERFACE
        $<BUILD_INTERFACE:-Wall -Wextra -Wpedantic>
    )
endif()

# Examples
if(LV_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Demos
if(LV_BUILD_DEMOS)
    add_subdirectory(demos)
endif()

# Tests
if(LV_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation (optional - only if not building as subdirectory)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    include(GNUInstallDirs)

    install(DIRECTORY include/lv
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
endif()
