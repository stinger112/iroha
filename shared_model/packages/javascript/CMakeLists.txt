cmake_minimum_required(VERSION 3.7.1)

project(irohanodejs)

get_filename_component(IROHA_HOME ../../../ ABSOLUTE)

add_compile_options(
	"-std=c++14" 
	"-fexceptions" 
	"-DDISABLE_BACKWARD" 
	"-O3"
	)

include(ExternalProject)

# set(EP_PREFIX ${CMAKE_CURRENT_LIST_DIR})
# message(STATUS ${EP_PREFIX})

# ExternalProject_Add(shared_model
#   SOURCE_DIR ${IROHA_HOME}
#   CMAKE_ARGS -DSWIG_NODE=ON -DSHARED_MODEL_DISABLE_COMPATIBILITY=ON -DSWIG_EXECUTABLE=/opt/swig/bin/swig
#   BUILD_BYPRODUCTS src/shared_model-build/shared_model/bindings/bindingsJAVASCRIPT_wrap.cxx
#                    src/shared_model-build/shared_model/bindings/libbindings.a
#                    src/shared_model-build/shared_model/bindings/irohanode.so
#   DOWNLOAD_COMMAND "" # remove download step
#   INSTALL_COMMAND "" # remove install step
#   TEST_COMMAND "" # remove test step
#   UPDATE_COMMAND "" # remove update step
#   )
# ExternalProject_Get_Property(shared_model source_dir binary_dir)

message(STATUS ${CMAKE_JS_LIB})
message(STATUS "Iroha home: " ${IROHA_HOME})
message(STATUS "VARIABLES: " ${source_dir} " " ${BINARY_DIR})

include_directories(
    ${IROHA_HOME}/shared_model
    ${IROHA_HOME}/libs
    ${IROHA_HOME}/external/src/martinmoene_optional/include
    ${IROHA_HOME}/irohad
    ${IROHA_HOME}/schema
	)

add_custom_command(
	OUTPUT iroha/shared_model/bindings/bindingsJAVASCRIPT_wrap.cxx iroha/shared_model/bindings/bindings.a iroha/shared_model/bindings/irohanode.so
	COMMAND cmake -H${IROHA_HOME} -Biroha -DSWIG_NODE=ON -DSHARED_MODEL_DISABLE_COMPATIBILITY=ON -DSWIG_EXECUTABLE=/opt/swig/bin/swig
	COMMAND cmake --build iroha/ --target irohanode -- -j2
	# DEPENDS shared_model
	)

# Build a shared library named after the project from the files in `src/`
# file(GLOB SOURCE_FILES "build_cmake/*.cc" "src/*.h")
add_library(${PROJECT_NAME} SHARED 
	iroha/shared_model/bindings/bindingsJAVASCRIPT_wrap.cxx
	)

# Gives our library file a .node extension without any "lib" prefix
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

# Essential include files to build a node addon,
# You should add this line in every CMake.js based project
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_JS_INC})

# Essential library files to link to a node addon
# You should add this line in every CMake.js based project
target_link_libraries(${PROJECT_NAME} 
	${CMAKE_JS_LIB}
	iroha/shared_model/bindings/bindings.a
	iroha/shared_model/bindings/irohanode.so
	)
