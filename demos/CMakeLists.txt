# Demos - full application demos

# E-Bike demo (C++ port of official lv_demos ebike)
file(GLOB EBIKE_GENERATED_SOURCES ebike/generated/*.c)
add_executable(ebike_demo
    ebike/main.cpp
    ebike/translations/lv_i18n.c
    ${EBIKE_GENERATED_SOURCES}
)
target_link_libraries(ebike_demo PRIVATE lv::lv lvgl)
target_include_directories(ebike_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Smartwatch demo (C++ port of official lv_demos smartwatch)
file(GLOB SMARTWATCH_GENERATED_SOURCES smartwatch/generated/*.c)
add_executable(smartwatch_demo
    smartwatch/main.cpp
    ${SMARTWATCH_GENERATED_SOURCES}
)
target_link_libraries(smartwatch_demo PRIVATE lv::lv lvgl)
target_include_directories(smartwatch_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# All demos
set(ALL_DEMOS ebike_demo smartwatch_demo)

# Try to find X11 first (native Linux)
find_package(X11 QUIET)
if(X11_FOUND)
    foreach(demo ${ALL_DEMOS})
        target_link_libraries(${demo} PRIVATE ${X11_LIBRARIES})
        target_include_directories(${demo} PRIVATE ${X11_INCLUDE_DIR})
    endforeach()
    message(STATUS "X11 found for demos - using native display")
endif()

# Also try SDL2 (cross-platform fallback)
find_package(SDL2 QUIET)
if(SDL2_FOUND OR TARGET SDL2::SDL2)
    foreach(demo ${ALL_DEMOS})
        if(TARGET SDL2::SDL2)
            target_link_libraries(${demo} PRIVATE SDL2::SDL2)
        else()
            target_link_libraries(${demo} PRIVATE ${SDL2_LIBRARIES})
            target_include_directories(${demo} PRIVATE ${SDL2_INCLUDE_DIRS})
        endif()
    endforeach()
    message(STATUS "SDL2 found for demos - available as fallback")
endif()
