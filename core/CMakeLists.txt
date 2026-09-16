# ######################################################################## #
# File:     /core/CMakeLists.txt
#
# Purpose:  CMake lists for the libver core library
#
# ######################################################################## #

add_library(libver STATIC
	backends/cargo.c
	backends/python.c
	backends/zig.c
	src/libver.c
	src/util.c
)

add_library(libver::libver ALIAS libver)

target_include_directories(libver
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${TOMLC17_INCLUDE_DIR}
)

# tomlc17 is an implementation dependency (not on the public headers).
# INTERFACE propagates the link to static-lib consumers.
target_link_libraries(libver
	PRIVATE
		${TOMLC17_LIBRARY}
	INTERFACE
		${TOMLC17_LIBRARY}
)

define_target_compile_options(libver)

if(BUILD_TESTING)

	add_subdirectory(test)
endif()


# ############################## end of file ############################# #
