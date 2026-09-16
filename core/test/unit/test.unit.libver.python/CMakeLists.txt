# ######################################################################## #
# File:     /core/test/unit/test.unit.libver.python/CMakeLists.txt
#
# Purpose:  CMake for Python literal-extract unit tests
#
# ######################################################################## #


define_automated_test_program(test.unit.libver.python entry.c)

target_include_directories(test.unit.libver.python
	PRIVATE
		${CMAKE_SOURCE_DIR}/core/src
)


# ############################## end of file ############################# #
