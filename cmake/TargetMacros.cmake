
macro(define_target_compile_options target_name)

	set(X_GCC_CUSTOM_WARNINGS_ "")

	if(X_GCC_CUSTOM_WARNINGS_TO_BE_SUPPRESSED)
		foreach(warning ${X_GCC_CUSTOM_WARNINGS_TO_BE_SUPPRESSED})

			list(APPEND X_GCC_CUSTOM_WARNINGS_ "-Wno-${warning}")
		endforeach()
	endif()

	set(X_MSVC_CUSTOM_WARNINGS_ "")

	if(X_MSVC_CUSTOM_WARNINGS_TO_BE_SUPPRESSED)
		foreach(warning ${X_MSVC_CUSTOM_WARNINGS_TO_BE_SUPPRESSED})

			list(APPEND X_MSVC_CUSTOM_WARNINGS_ "/wd${warning}")
		endforeach()
	endif()

	target_compile_options(${target_name}
		PRIVATE
			$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
				-Werror -Wall -Wextra -pedantic

				${GCC_WARN_NO_cxx11_long_long}
				${X_GCC_CUSTOM_WARNINGS_}
			>
			$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
				-Wno-unused-lambda-capture
			>
			$<$<CXX_COMPILER_ID:MSVC>:
				/WX /W4

				${X_MSVC_CUSTOM_WARNINGS_}
			>
	)
endmacro(define_target_compile_options)


function(define_automated_test_program program_name entry_point_source_name)

	add_executable(${program_name}
		${entry_point_source_name}
	)

	target_link_libraries(${program_name}
		PRIVATE
			libver
			$<$<BOOL:${STLSoft_FOUND}>:STLSoft::STLSoft>
			$<$<BOOL:${shwild_FOUND}>:shwild::core>
			$<IF:$<VERSION_LESS:${xTests_VERSION},"0.23">,xTests::xTests.core,xTests::core>
	)

	if(WIN32)

		target_link_libraries(${program_name}
			PRIVATE
				wininet
		)
	endif(WIN32)

	define_target_compile_options(${program_name})

	add_test(
		NAME ${program_name}
		COMMAND ${program_name}
	)
endfunction(define_automated_test_program)


function(define_example_program program_name entry_point_source_name)

	add_executable(${program_name}
		${entry_point_source_name}
	)

	target_link_libraries(${program_name}
		PRIVATE
			libver
			$<$<BOOL:${STLSoft_FOUND}>:STLSoft::STLSoft>
	)

	if(WIN32)

		target_link_libraries(${program_name}
			PRIVATE
				wininet
		)
	endif(WIN32)

	define_target_compile_options(${program_name})
endfunction(define_example_program)


function(define_scratch_program program_name entry_point_source_name)

	add_executable(${program_name}
		${entry_point_source_name}
	)

	target_link_libraries(${program_name}
		PRIVATE
			libver
			Diagnosticism::core
	)

	if(WIN32)

		target_link_libraries(${program_name}
			PRIVATE
				wininet
		)
	endif(WIN32)

	define_target_compile_options(${program_name})
endfunction(define_scratch_program)


function(define_cli_program program_name entry_point_source_name tool_name)

	add_executable(${program_name}
		${entry_point_source_name}
	)

	# Keep CMake target names distinct from the static library target `libver`.
	set_target_properties(${program_name} PROPERTIES
		OUTPUT_NAME "${tool_name}"
	)

	target_compile_definitions(${program_name}
		PRIVATE
			LIBVER_TOOL_NAME=\"${tool_name}\"
	)

	target_link_libraries(${program_name}
		PRIVATE
			libver
			sistools-common-c::core
			CLASP::core
			cstring::core
			collect-c::core
			Diagnosticism::core
			$<$<BOOL:${STLSoft_FOUND}>:STLSoft::STLSoft>
	)

	if(WIN32)

		target_link_libraries(${program_name}
			PRIVATE
				wininet
		)
	endif(WIN32)

	define_target_compile_options(${program_name})

	install(TARGETS ${program_name}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
endfunction(define_cli_program)


# ############################## end of file ############################# #
