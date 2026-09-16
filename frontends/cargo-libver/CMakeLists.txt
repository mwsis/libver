# ######################################################################## #
# File:     /frontends/cargo-libver/CMakeLists.txt
#
# Purpose:  CMake lists for the cargo-libver frontend
#
# ######################################################################## #

define_cli_program(frontend.cargo-libver "${CMAKE_SOURCE_DIR}/cli/common/entry.c" "cargo-libver")

target_compile_definitions(frontend.cargo-libver
	PRIVATE
		LIBVER_CLI_SCHEMES=LIBVER_SCHEME_CARGO
		"LIBVER_CLI_DESCRIPTION=\"Discover the Cargo package version of a project\""
)

if(BUILD_TESTING)

	add_test(
		NAME frontend.cargo-libver.cargo-only
		COMMAND frontend.cargo-libver "${CMAKE_SOURCE_DIR}/core/test/fixtures/cargo-only"
	)
	set_tests_properties(frontend.cargo-libver.cargo-only PROPERTIES
		PASS_REGULAR_EXPRESSION "scheme:  cargo[\r\n]+version: 1\\.2\\.3"
	)

	add_test(
		NAME frontend.cargo-libver.zig-only
		COMMAND frontend.cargo-libver "${CMAKE_SOURCE_DIR}/core/test/fixtures/zig-only"
	)
	set_tests_properties(frontend.cargo-libver.zig-only PROPERTIES
		WILL_FAIL TRUE
	)
endif(BUILD_TESTING)


# ############################## end of file ############################# #
