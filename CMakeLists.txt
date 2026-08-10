# ######################################################################## #
# File:     /CMakeLists.txt
#
# Purpose:  Top-level CMake lists file for libver
#
# Created:  10th August 2026
# Updated:  10th August 2026
#
# ######################################################################## #


# ##########################################################
# CMake

cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# require out-of-source builds
file(TO_CMAKE_PATH "${CMAKE_CURRENT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")

	message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

# directory for CMake specific extensions and source files.
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})


# ##########################################################
# project

project(libver
	DESCRIPTION "libver discovers the definitive version of a software project from its directory."
	HOMEPAGE_URL "https://github.com/synesissoftware/libver"
	LANGUAGES C CXX
)

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)

# handle version number
set(RX_PROJ_TAG "${PROJECT_NAME_UPPER}")
set(RX_WS "[ \t]")
file(READ "${CMAKE_SOURCE_DIR}/core/include/${PROJECT_NAME_LOWER}/${PROJECT_NAME_LOWER}.h" _header_file)
string(REGEX MATCH "#${RX_WS}*define${RX_WS}+_?${RX_PROJ_TAG}_VER_MAJOR${RX_WS}+([0-9]+)" MAJOR_DUMMY ${_header_file})
set(_VERSION_MAJOR ${CMAKE_MATCH_1})
string(REGEX MATCH "#${RX_WS}*define${RX_WS}+_?${RX_PROJ_TAG}_VER_MINOR${RX_WS}+([0-9]+)" MINOR_DUMMY ${_header_file})
set(_VERSION_MINOR ${CMAKE_MATCH_1})
string(REGEX MATCH "#${RX_WS}*define${RX_WS}+_?${RX_PROJ_TAG}_VER_(PATCH|REVISION)${RX_WS}+([0-9]+)" PATCH_DUMMY ${_header_file})
set(_VERSION_PATCH ${CMAKE_MATCH_2})

# set project version number here
set(PROJECT_VERSION_MAJOR ${_VERSION_MAJOR})
set(PROJECT_VERSION_MINOR ${_VERSION_MINOR})
set(PROJECT_VERSION_PATCH ${_VERSION_PATCH})
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if(MSVC)

	if(MSVC_USE_MT)

		set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	endif(MSVC_USE_MT)
else(MSVC)

	if(MSVC_USE_MT)

		# absorb unused-option warning when prepare_cmake.sh passes the flag
	endif(MSVC_USE_MT)
endif(MSVC)


# ##########################################################
# includes

include(BuildType)
include(LanguageFullVersion)
include(TargetMacros)


# ##########################################################
# options

option(BUILD_CLI "Build the shared libver CLI" ON)
option(BUILD_FRONTENDS "Build ecosystem frontends (e.g. cargo-libver)" ON)
option(BUILD_TESTING "Build tests" ON)

message(STATUS "libver options: BUILD_CLI=${BUILD_CLI} BUILD_FRONTENDS=${BUILD_FRONTENDS} BUILD_TESTING=${BUILD_TESTING}")


# ##########################################################
# dependencies
#
# required (when BUILD_TESTING):
# - Diagnosticism - scratch tests (version-string formatting);
# - STLSoft - required for xTests-based unit tests;
# - xTests - required for unit tests;


# ######################################
# Diagnosticism (required for scratch tests; version-string formatting)

if(BUILD_TESTING)

	# Optional install/prefix hints (Synesis convention):
	# 1. CMake variable DIAGNOSTICISM; otherwise
	# 2. environment variable DIAGNOSTICISM; otherwise
	# 3. default find_package() search paths

	if(DEFINED DIAGNOSTICISM)

		message("-- DIAGNOSTICISM provided as CMake variable with value '${DIAGNOSTICISM}'")

		list(PREPEND CMAKE_PREFIX_PATH "${DIAGNOSTICISM}")
		set(diagnosticism_ROOT "${DIAGNOSTICISM}")
	elseif(DEFINED ENV{DIAGNOSTICISM})

		message("-- DIAGNOSTICISM provided as environment variable with value '$ENV{DIAGNOSTICISM}'")

		list(PREPEND CMAKE_PREFIX_PATH "$ENV{DIAGNOSTICISM}")
		set(diagnosticism_ROOT "$ENV{DIAGNOSTICISM}")
	endif()

	# Diagnosticism >= 0.3.1 exports Diagnosticism::core with no transitive
	# package dependencies (STLSoft is test-only for Diagnosticism itself).
	find_package(diagnosticism REQUIRED)

	message("-- CMake package diagnosticism found (version ${diagnosticism_VERSION})")
endif(BUILD_TESTING)


# ######################################
# STLSoft

if(BUILD_TESTING)

	if(DEFINED STLSOFT)

		message("-- STLSOFT provided as CMake variable with value '${STLSOFT}'")

		set(STLSOFT_INCLUDE_DIR ${STLSOFT}/include)
	elseif(DEFINED ENV{STLSOFT})

		message("-- STLSOFT provided as environment variable with value '$ENV{STLSOFT}'")

		set(STLSOFT_INCLUDE_DIR $ENV{STLSOFT}/include)
	else()

		set(STLSoft_REQUIRED_VERSION_ 1.11)

		find_package(STLSoft ${STLSoft_REQUIRED_VERSION_} REQUIRED)

		message("-- CMake package STLSoft found (version ${STLSoft_VERSION}; ${STLSoft_REQUIRED_VERSION_} requested)")
	endif()

	if(DEFINED STLSOFT_INCLUDE_DIR OR DEFINED STLSoft_FOUND)

		include_directories(${STLSOFT_INCLUDE_DIR})

		add_compile_definitions(HAS_STLSOFT)
		add_compile_definitions(HAS_STLSoft)
	endif()
endif(BUILD_TESTING)


# ######################################
# xTests

if(BUILD_TESTING)

	set(xTests_REQUIRED_VERSION_ 0.26)

	find_package(xTests ${xTests_REQUIRED_VERSION_} REQUIRED)

	message("-- CMake package xTests found (version ${xTests_VERSION}; ${xTests_REQUIRED_VERSION_} requested)")
endif(BUILD_TESTING)


# ##########################################################
# includes - 2

if(BUILD_TESTING)

	include(CTest)
endif(BUILD_TESTING)


# ##########################################################
# targets

add_subdirectory(core)

# CLI and frontend directories are scaffolded; wire add_subdirectory when ready.
if(BUILD_CLI)

	# add_subdirectory(cli/libver)
endif()

if(BUILD_FRONTENDS)

	# add_subdirectory(frontends/cargo-libver)
endif()

# ############################## end of file ############################# #
