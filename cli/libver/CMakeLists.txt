# ######################################################################## #
# File:     /cli/libver/CMakeLists.txt
#
# Purpose:  CMake lists for the shared libver CLI
#
# ######################################################################## #


define_cli_program(cli.libver "${CMAKE_SOURCE_DIR}/cli/common/entry.c" "libver")

if(BUILD_TESTING)

	add_test(
		NAME cli.libver.cargo-only
		COMMAND cli.libver "${CMAKE_SOURCE_DIR}/core/test/fixtures/cargo-only"
	)
	set_tests_properties(cli.libver.cargo-only PROPERTIES
		PASS_REGULAR_EXPRESSION "scheme:  cargo[\r\n]+version: 1\\.2\\.3"
	)

	add_test(
		NAME cli.libver.cwd
		COMMAND cli.libver
	)
	set_tests_properties(cli.libver.cwd PROPERTIES
		PASS_REGULAR_EXPRESSION "scheme:  cargo[\r\n]+version: 1\\.2\\.3"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/core/test/fixtures/cargo-only"
	)

	add_test(
		NAME cli.libver.empty
		COMMAND cli.libver "${CMAKE_SOURCE_DIR}/core/test/fixtures/empty"
	)
	set_tests_properties(cli.libver.empty PROPERTIES
		WILL_FAIL TRUE
	)

	add_test(
		NAME cli.libver.zig-only
		COMMAND cli.libver "${CMAKE_SOURCE_DIR}/core/test/fixtures/zig-only"
	)
	set_tests_properties(cli.libver.zig-only PROPERTIES
		PASS_REGULAR_EXPRESSION "scheme:  zig[\r\n]+version: 0\\.4\\.5"
	)
endif(BUILD_TESTING)


# ############################## end of file ############################# #
