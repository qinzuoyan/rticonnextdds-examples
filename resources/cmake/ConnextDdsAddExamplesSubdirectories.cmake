# (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.

#[[.rst
.. connextdds_add_examples_subdirectories:

ConnextDdsAddExamplesSubdirectories
-----------------------------------

Function to build all the language directories under the provided examples.

::

    connextdds_add_examples_subdirectories(
        EXAMPLES example1 [...]
        [SKIP condition]
        [SKIP_MESSAGE message]
    )

Add a bunch of examples to be built. This method adds subdirectories that
contains an example with a CMakeList.txt and generates the build system
for all of them.

``EXAMPLES`` (required):
    List of examples names that will be compiled if SKIP is not defined.
``SKIP`` (optional):
    If defined, the examples will be skipped.
``SKIP_MESSAGE`` (optional):
    If the examples are going to be skipped, prints this message after
    ``Skipping examples because``.
]]

include(CMakeParseArguments)
include(ConnextDdsArgumentChecks)

function(connextdds_add_examples_subdirectories)
    set(optional_args SKIP)
    set(single_value_args SKIP_MESSAGE)
    set(multi_value_args EXAMPLES)
    cmake_parse_arguments(_ADD_EXAMPLES_ARGS
        "${optional_args}"
        "${single_value_args}"
        "${multi_value_args}"
        ${ARGN}
    )
    connextdds_check_required_arguments(
        _ADD_EXAMPLES_ARGS_EXAMPLES
    )

    set(LANGUAGE_FOLDER_NAMES c c++98 c++ c++03 c++11)

    # Check if all directories exist 
    foreach(example ${_ADD_EXAMPLES_ARGS_EXAMPLES})
        if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${example}")
            message(FATAL_ERROR "${example} does not exist")
        endif()
    endforeach()

    # Check if skip
    if(_ADD_EXAMPLES_ARGS_SKIP)
        string(REPLACE ";" "\n\t- " SKIPPED_EXAMPLES "${_ADD_EXAMPLES_ARGS_EXAMPLES}")
        message(
            STATUS
                "Skipping examples because ${_ADD_EXAMPLES_ARGS_SKIP_MESSAGE}. "
                "The skipped examples are: \n\t- ${SKIPPED_EXAMPLES}"
        )
        return()
    endif()

    # Add existing examples
    foreach(example_name ${_ADD_EXAMPLES_ARGS_EXAMPLES})
        foreach(language_folder ${LANGUAGE_FOLDER_NAMES})
            if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${example_name}/${language_folder}")
                add_subdirectory(
                    "${CMAKE_CURRENT_SOURCE_DIR}/${example_name}/${language_folder}"
                )
            endif()
        endforeach()
    endforeach()
endfunction()
