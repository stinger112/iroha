add_library(outcome INTERFACE IMPORTED)

find_path(outcome_INCLUDE_DIR outcome.hpp)
mark_as_advanced(outcome_INCLUDE_DIR)

find_package_handle_standard_args(outcome DEFAULT_MSG
    outcome
    )

set(URL https://github.com/ned14/outcome.git)
set(VERSION 256bcf20946d9a5afe5ccb42571b0d45950cc0aa)
set_target_description(outcome "Data structure for returning result or error" ${URL} ${VERSION})

if (NOT ned14_outcome_FOUND)
    externalproject_add(ned14_outcome
        GIT_REPOSITORY ${URL}
        GIT_TAG        ${VERSION}
        CONFIGURE_COMMAND "" # remove configure step
        BUILD_COMMAND "" # remove build step
        INSTALL_COMMAND "" # remove install step
        TEST_COMMAND "" # remove test step
        UPDATE_COMMAND "" # remove update step
        )
    externalproject_get_property(ned14_outcome source_dir)
    set(outcome_INCLUDE_DIR ${source_dir}/include)
    file(MAKE_DIRECTORY ${outcome_INCLUDE_DIR})

    add_dependencies(outcome ned14_outcome)
endif ()

set_target_properties(outcome PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${outcome_INCLUDE_DIR}
        )