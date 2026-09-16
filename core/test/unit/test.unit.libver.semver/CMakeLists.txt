# ######################################################################## #
# File:     /core/test/unit/test.unit.libver.semver/CMakeLists.txt
#
# Purpose:  CMake for SemVer parse unit tests
#
# ######################################################################## #


define_automated_test_program(test.unit.libver.semver entry.c)

target_include_directories(test.unit.libver.semver
	PRIVATE
		${CMAKE_SOURCE_DIR}/core/src
)


# ############################## end of file ############################# #
