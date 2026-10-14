cmake_minimum_required(VERSION 3.16)
project(OpenRGB_ApexProMini LANGUAGES CXX)
enable_testing()
add_executable(apex_pro_mini_protocol_test tests/protocol_test.cpp)
target_include_directories(apex_pro_mini_protocol_test PRIVATE include)
target_compile_features(apex_pro_mini_protocol_test PRIVATE cxx_std_17)
add_test(NAME apex_pro_mini_protocol COMMAND apex_pro_mini_protocol_test)

option(BUILD_OPENRGB_PLUGIN "Build Qt plugin against OpenRGB source tree" OFF)
if(BUILD_OPENRGB_PLUGIN)
    if(NOT DEFINED OPENRGB_SOURCE_DIR)
        message(FATAL_ERROR "Set OPENRGB_SOURCE_DIR to the matching OpenRGB source checkout")
    endif()
    if(NOT EXISTS "${OPENRGB_SOURCE_DIR}/OpenRGBPluginInterface.h"
       OR NOT EXISTS "${OPENRGB_SOURCE_DIR}/RGBController/RGBControllerInterface.h")
        message(FATAL_ERROR
            "OPENRGB_SOURCE_DIR must point to an OpenRGB source checkout, "
            "not this plugin repository. Expected OpenRGBPluginInterface.h and "
            "RGBController/RGBControllerInterface.h in ${OPENRGB_SOURCE_DIR}")
    endif()
    set(OPENRGB_QT_MAJOR "5" CACHE STRING "Qt major version used to compile the OpenRGB executable (5 or 6)")
    set_property(CACHE OPENRGB_QT_MAJOR PROPERTY STRINGS 5 6)
    if(NOT OPENRGB_QT_MAJOR MATCHES "^[56]$")
        message(FATAL_ERROR "OPENRGB_QT_MAJOR must be 5 or 6")
    endif()
    find_package(Qt${OPENRGB_QT_MAJOR} REQUIRED COMPONENTS Core Widgets)
    message(STATUS "Building plugin against Qt${OPENRGB_QT_MAJOR}; this must match the OpenRGB executable")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(HIDAPI REQUIRED IMPORTED_TARGET hidapi-hidraw)
    add_executable(apex_plugin_metadata_check tests/plugin_metadata_check.cpp)
    target_link_libraries(apex_plugin_metadata_check PRIVATE Qt${OPENRGB_QT_MAJOR}::Core)
    set(CMAKE_AUTOMOC ON)
    add_library(OpenRGB_ApexProMini MODULE src/ApexProMiniPlugin.cpp src/ApexProMiniPlugin.h)
    target_compile_features(OpenRGB_ApexProMini PRIVATE cxx_std_17)
    target_include_directories(OpenRGB_ApexProMini PRIVATE
        include
        "${OPENRGB_SOURCE_DIR}"
        "${OPENRGB_SOURCE_DIR}/RGBController"
        "${OPENRGB_SOURCE_DIR}/dependencies"
    )
    target_link_libraries(OpenRGB_ApexProMini PRIVATE Qt${OPENRGB_QT_MAJOR}::Core Qt${OPENRGB_QT_MAJOR}::Widgets PkgConfig::HIDAPI)
    set_target_properties(OpenRGB_ApexProMini PROPERTIES PREFIX "" OUTPUT_NAME "OpenRGB-ApexProMini")
    add_test(NAME apex_plugin_metadata
        COMMAND apex_plugin_metadata_check $<TARGET_FILE:OpenRGB_ApexProMini>)
endif()
