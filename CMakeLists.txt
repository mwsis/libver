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
include(LibverFindPackage)
include(TargetMacros)
include(GNUInstallDirs)


# ##########################################################
# options

option(BUILD_CLI "Build the shared libver CLI" ON)
option(BUILD_FRONTENDS "Build ecosystem frontends (e.g. cargo-libver)" ON)
option(BUILD_TESTING "Build tests" ON)
option(LIBVER_WITH_PANTHEIOS "Locate Pantheios (wired for later; not linked yet)" OFF)

message(STATUS "libver options: BUILD_CLI=${BUILD_CLI} BUILD_FRONTENDS=${BUILD_FRONTENDS} BUILD_TESTING=${BUILD_TESTING} LIBVER_WITH_PANTHEIOS=${LIBVER_WITH_PANTHEIOS}")

set(LIBVER_NEED_CLI_DEPS OFF)
if(BUILD_CLI OR BUILD_FRONTENDS)

	set(LIBVER_NEED_CLI_DEPS ON)
endif()


# ##########################################################
# dependencies
#
# always:
# - Diagnosticism;
# - STLSoft (CLASP / sistools-common-c / xTests);
#
# when BUILD_CLI or BUILD_FRONTENDS:
# - CLASP;
# - cstring;
# - collect-c;
# - sistools-common-c;
#
# optional (CMake wiring only for now):
# - Pantheios;
#
# when BUILD_TESTING:
# - xTests;


# ######################################
# prefix hints (install prefixes or build trees)

libver_prepend_prefix_path_if_set(DIAGNOSTICISM)
libver_prepend_prefix_path_if_set(SISTOOLS_COMMON_C)
libver_prepend_prefix_path_if_set(COLLECT_C)
libver_prepend_prefix_path_if_set(PANTHEIOS)


# ######################################
# STLSoft

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


# ######################################
# Diagnosticism

set(DIAGNOSTICISM_REQUIRED_VERSION_ 0.3)

find_package(Diagnosticism ${DIAGNOSTICISM_REQUIRED_VERSION_} REQUIRED)

message("-- CMake package Diagnosticism found (version ${Diagnosticism_VERSION}; ${DIAGNOSTICISM_REQUIRED_VERSION_} requested)")


# ######################################
# CLI stack

if(LIBVER_NEED_CLI_DEPS)

	# ######################################
	# CLASP

	set(CLASP_REQUIRED_VERSION_ 0.15)

	find_package(CLASP ${CLASP_REQUIRED_VERSION_} REQUIRED)

	message("-- CMake package CLASP found (version ${CLASP_VERSION}; ${CLASP_REQUIRED_VERSION_} requested)")


	# ######################################
	# collect-c

	set(collect_c_REQUIRED_VERSION_ 0.1)

	find_package(collect-c ${collect_c_REQUIRED_VERSION_} REQUIRED)

	message("-- CMake package collect-c found (version ${collect_c_VERSION}; ${collect_c_REQUIRED_VERSION_} requested)")


	# ######################################
	# cstring

	set(cstring_REQUIRED_VERSION_ 4)

	find_package(cstring ${cstring_REQUIRED_VERSION_} REQUIRED)

	message("-- CMake package cstring found (version ${cstring_VERSION}; ${cstring_REQUIRED_VERSION_} requested)")


	# ######################################
	# sistools-common-c

	set(sistools_common_c_REQUIRED_VERSION_ 0.1)

	find_package(sistools-common-c ${sistools_common_c_REQUIRED_VERSION_} REQUIRED)

	message("-- CMake package sistools-common-c found (version ${sistools_common_c_VERSION}; ${sistools_common_c_REQUIRED_VERSION_} requested)")
endif(LIBVER_NEED_CLI_DEPS)


# ######################################
# Pantheios

if(LIBVER_WITH_PANTHEIOS)

	libver_find_config_package(Pantheios 1.0 TRUE
		HINTS
			"$ENV{HOME}/dev/synesissoftware/forks/freelibs/Pantheios/Pantheios/_build"
			"$ENV{HOME}/dev/synesissoftware/forks/freelibs/Pantheios/Pantheios"
	)
	set(LIBVER_HAS_PANTHEIOS TRUE)
else()

	libver_find_config_package(Pantheios 1.0 FALSE
		HINTS
			"$ENV{HOME}/dev/synesissoftware/forks/freelibs/Pantheios/Pantheios/_build"
			"$ENV{HOME}/dev/synesissoftware/forks/freelibs/Pantheios/Pantheios"
	)
	if(Pantheios_FOUND)

		set(LIBVER_HAS_PANTHEIOS TRUE)
	else()

		set(LIBVER_HAS_PANTHEIOS FALSE)
	endif()
endif()

message(STATUS "libver Pantheios: LIBVER_HAS_PANTHEIOS=${LIBVER_HAS_PANTHEIOS} (not linked yet; libCLImate later)")


# ######################################
# xTests

if(BUILD_TESTING)

	set(xTests_REQUIRED_VERSION_ 0.26)

	find_package(xTests ${xTests_REQUIRED_VERSION_} REQUIRED)

	message("-- CMake package xTests found (version ${xTests_VERSION}; ${xTests_REQUIRED_VERSION_} requested)")

	include(CTest)
endif(BUILD_TESTING)


# ##########################################################
# targets

add_subdirectory(core)

if(BUILD_CLI)

	add_subdirectory(cli/libver)
endif()

if(BUILD_FRONTENDS)

	add_subdirectory(frontends/cargo-libver)
endif()


# ############################## end of file ############################# #
