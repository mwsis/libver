# ######################################################################## #
# File:     /core/test/scratch/rust/CMakeLists.txt
#
# Purpose:  CMake for the Cargo.toml / tomlc17 scratch program
#
# ######################################################################## #

add_executable(test.scratch.rust
	entry.c
)

target_include_directories(test.scratch.rust
	PRIVATE
		${TOMLC17_INCLUDE_DIR}
)

target_link_libraries(test.scratch.rust
	PRIVATE
		${TOMLC17_LIBRARY}
)

define_target_compile_options(test.scratch.rust)


# ############################## end of file ############################# #
