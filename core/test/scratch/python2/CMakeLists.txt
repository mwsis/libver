# ######################################################################## #
# File:     /core/test/scratch/python2/CMakeLists.txt
#
# Purpose:  CMake for the Python 2 setup.py / __init__.py scratch program
#
# ######################################################################## #

add_executable(test.scratch.python2
	entry.c
)

# recls platform.h recognises unix/UNIX (Apple Clang does not define them).
if(APPLE OR UNIX)

	target_compile_definitions(test.scratch.python2
		PRIVATE
			UNIX
			unix
	)
endif()

target_link_libraries(test.scratch.python2
	PRIVATE
		recls::core
)

define_target_compile_options(test.scratch.python2)


# ############################## end of file ############################# #
