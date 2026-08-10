# ######################################################################## #
# File:     /core/test/CMakeLists.txt
#
# Purpose:  CMake lists for libver core tests
#
# ######################################################################## #


foreach(d component performance scratch unit)

	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${d}/CMakeLists.txt")

		add_subdirectory(${d})
	endif()
endforeach()


# ############################## end of file ############################# #
