# ######################################################################## #
# File:     /core/test/scratch/python3/CMakeLists.txt
#
# Purpose:  CMake for the pyproject.toml / tomlc17 scratch program
#
# ######################################################################## #

add_executable(test.scratch.python3
	entry.c
)

target_include_directories(test.scratch.python3
	PRIVATE
		${TOMLC17_INCLUDE_DIR}
)

target_link_libraries(test.scratch.python3
	PRIVATE
		${TOMLC17_LIBRARY}
)

define_target_compile_options(test.scratch.python3)


# ############################## end of file ############################# #
