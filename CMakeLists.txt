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
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(HIDAPI REQUIRED IMPORTED_TARGET hidapi-hidraw)
    set(CMAKE_AUTOMOC ON)
    add_library(OpenRGB_ApexProMini MODULE src/ApexProMiniPlugin.cpp src/ApexProMiniPlugin.h)
    target_compile_features(OpenRGB_ApexProMini PRIVATE cxx_std_17)
    target_include_directories(OpenRGB_ApexProMini PRIVATE
        include
        "${OPENRGB_SOURCE_DIR}"
        "${OPENRGB_SOURCE_DIR}/RGBController"
        "${OPENRGB_SOURCE_DIR}/dependencies"
    )
    target_link_libraries(OpenRGB_ApexProMini PRIVATE Qt6::Core Qt6::Widgets PkgConfig::HIDAPI)
    set_target_properties(OpenRGB_ApexProMini PROPERTIES PREFIX "" OUTPUT_NAME "OpenRGB-ApexProMini")
endif()
