# ######################################################################## #
# File:     /cmake/LibverFindPackage.cmake
#
# Purpose:  Helpers for locating Synesis / sistools CMake packages, including
#           local build-tree exports under $HOME/dev/...
#
# ######################################################################## #

function(libver_prepend_prefix_path_if_set var_name)
	if(DEFINED ${var_name})

		list(PREPEND CMAKE_PREFIX_PATH "${${var_name}}")
		set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" PARENT_SCOPE)
		message("-- ${var_name} provided as CMake variable with value '${${var_name}}'")
	elseif(DEFINED ENV{${var_name}})

		list(PREPEND CMAKE_PREFIX_PATH "$ENV{${var_name}}")
		set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" PARENT_SCOPE)
		message("-- ${var_name} provided as environment variable with value '$ENV{${var_name}}'")
	endif()
endfunction()


function(libver_find_config_package
	package_name
	required_version
	is_required
)
	# Extra args: HINTS ...
	set(options)
	set(oneValueArgs)
	set(multiValueArgs HINTS)
	cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	set(_req_flag)
	if(is_required)

		set(_req_flag REQUIRED)
	else()

		set(_req_flag QUIET)
	endif()

	find_package(${package_name} ${required_version} CONFIG ${_req_flag}
		HINTS
			${ARG_HINTS}
		PATH_SUFFIXES
			cmake
			lib/cmake
			lib/cmake/${package_name}
	)

	if(${package_name}_FOUND)

		message("-- CMake package ${package_name} found (version ${${package_name}_VERSION}; ${required_version} requested)")
		set(${package_name}_FOUND TRUE PARENT_SCOPE)
	elseif(is_required)

		message(FATAL_ERROR "CMake package ${package_name} (${required_version}) is required but was not found. Set ${package_name}_ROOT / CMAKE_PREFIX_PATH, or install the package.")
	else()

		message("-- CMake package ${package_name} not found (optional)")
		set(${package_name}_FOUND FALSE PARENT_SCOPE)
	endif()
endfunction()


# ############################## end of file ############################# #
