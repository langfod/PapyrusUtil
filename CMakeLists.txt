cmake_minimum_required(VERSION 4.1)
set(VCPKG_TARGET_TRIPLET "x64-windows-static" CACHE STRING "")

# ---- Project settings ----
project(
    PapyrusUtil
    VERSION 4.7.0
    LANGUAGES CXX
)

# ---- Build paths ----
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
set(BUILD_ROOT "${CMAKE_SOURCE_DIR}/build")

if(NOT DEFINED CMAKE_COMPILE_JOBS)
    set(CMAKE_COMPILE_JOBS 8)
endif()

if(NOT DEFINED CMAKE_LINK_JOBS)
    set(CMAKE_LINK_JOBS 2)
endif()

# Include compiler flags configuration
include(CompilerFlags)

# ---- Find external packages ----
find_package(Boost REQUIRED COMPONENTS filesystem random)
find_package(jsoncpp CONFIG REQUIRED)

# Setup external dependencies (built from source: CommonLibSSE)
include(ExternalDependencies)

# ---- Collect source files ----
file(GLOB_RECURSE SOURCES
    CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/*.h"
)


# ---- Create source groups for IDE ----
source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PREFIX "Source Files"
    FILES ${SOURCES}
)

# ---- Create the SKSE plugin using CommonLibSSE-NG ----
add_commonlibsse_plugin(${PROJECT_NAME}
    NAME "PapyrusUtil"
    AUTHOR "Ashal"
    EMAIL "Ashal@loverslab.com"
    VERSION "${PROJECT_VERSION}"
    USE_ADDRESS_LIBRARY
    SOURCES ${SOURCES}
)

# ---- Include directories ----
target_include_directories(${PROJECT_NAME}
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# ---- Precompiled header ----
target_precompile_headers(${PROJECT_NAME}
    PRIVATE
    src/PCH.h
)

# ---- Compiler definitions ----
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
    _CRT_SECURE_NO_WARNINGS
    _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
)

# ---- Link libraries ----
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Boost::filesystem
    Boost::random
    Boost::headers
    JsonCpp::JsonCpp
)

# ---- Set output directory ----
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# Post-build: Create distribution folder structure
set(DIST_ROOT "${CMAKE_SOURCE_DIR}/dist")
set(DIST_VERSION_DIR "${DIST_ROOT}/PapyrusUtil_${PROJECT_VERSION}")
set(DIST_PapyrusUtil_Scripts_DIR "${DIST_VERSION_DIR}/Scripts")
set(DIST_SKSE_PLUGINS_DIR "${DIST_VERSION_DIR}/SKSE/plugins")
set(DIST_SDK_DIR "${DIST_VERSION_DIR}/SDK")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    # Create distribution directories
    COMMAND ${CMAKE_COMMAND} -E make_directory "${DIST_PapyrusUtil_Scripts_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${DIST_SKSE_PLUGINS_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${DIST_SDK_DIR}"

    # Copy assets folder contents to PapyrusUtil folder
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/Scripts/"
    "${DIST_PapyrusUtil_Scripts_DIR}"

    # Copy PapyrusUtil DLL to SKSE/plugins
    COMMAND ${CMAKE_COMMAND} -E copy
    "$<TARGET_FILE:${PROJECT_NAME}>"
    "${DIST_SKSE_PLUGINS_DIR}/"

    # Copy PapyrusUtil PDB to SKSE/plugins (if exists)
    COMMAND ${CMAKE_COMMAND} -E $<IF:$<BOOL:$<TARGET_PDB_FILE:${PROJECT_NAME}>>,copy,true>
    "$<$<BOOL:$<TARGET_PDB_FILE:${PROJECT_NAME}>>:$<TARGET_PDB_FILE:${PROJECT_NAME}>>"
    "${DIST_SKSE_PLUGINS_DIR}/"

    # Copy public API header to SDK folder for other plugin developers
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_SOURCE_DIR}/include/PapyrusUtilAPI.h"
    "${DIST_SDK_DIR}/"

    COMMENT "Creating distribution folder: dist/PapyrusUtil_${PROJECT_VERSION}"
)
