# ######################################################################## #
# File:     /core/CMakeLists.txt
#
# Purpose:  CMake lists for the libver core library
#
# ######################################################################## #

add_library(libver STATIC
	src/libver.c
)

add_library(libver::libver ALIAS libver)

target_include_directories(libver
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
)

define_target_compile_options(libver)

if(BUILD_TESTING)

	add_subdirectory(test)
endif()


# ############################## end of file ############################# #
