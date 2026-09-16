# ######################################################################## #
# File:     /core/test/component/test.component.libver.find/CMakeLists.txt
#
# Purpose:  CMake for the libver_find component tests (fixture trees)
#
# ######################################################################## #

define_automated_test_program(test.component.libver.find entry.c)

target_compile_definitions(test.component.libver.find
	PRIVATE
		LIBVER_FIXTURES_DIR="${CMAKE_SOURCE_DIR}/core/test/fixtures"
)


# ############################## end of file ############################# #
