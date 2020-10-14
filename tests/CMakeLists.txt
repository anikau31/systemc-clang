# Test CMakeFile

configure_file( ClangArgs.h.in ClangArgs.h)

# updating submodules
if(V_TESTS_ON)
  # pull and update submodule
  find_package(Git QUIET)
  if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
  endif()

  if(NOT EXISTS "${PROJECT_SOURCE_DIR}/tests/data/verilog-conversion/.git")
      message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
  endif()

  file(COPY verilog-conversion/run-verilog-tests.py DESTINATION ${CMAKE_BINARY_DIR}/)
endif()

# Test files so far are t#.cpp
# Don't use globbing as it doesn't recognize recompiles.
# file(GLOB TEST_FILES "t[0-9].cpp")

# copy the directory of data to test build
file(COPY data DESTINATION ${CMAKE_BINARY_DIR}/tests/ )

# copy some of the examples that are necessary for testing
# be careful about the filename as they may collide 
file(COPY ../examples/llnl-examples DESTINATION ${CMAKE_BINARY_DIR}/tests/data )

set(TEST_DIR "tests")

#add_subdirectory(unit)
#add_subdirectory(matchers)
add_subdirectory(parsing)

# Plugins
add_subdirectory(hdl)


include(CTest)

# catch2 may have headers colliding with other local files, 
# so we can use -iquote instead of -I to include files in src folder

# for pytest tests
if(V_TESTS_ON)
  # integrate into ctest
  add_test(NAME verilog-tests
    COMMAND  ${CMAKE_COMMAND} -E env SYSTEMC_CLANG=${PROJECT_SOURCE_DIR} ${PYTHON_EXECUTABLE} -m pytest --color=yes ${CMAKE_CURRENT_SOURCE_DIR})
  add_test(NAME verilog-sanity-tests
    COMMAND  ${CMAKE_COMMAND} -E env SYSTEMC_CLANG=${PROJECT_SOURCE_DIR} ${PYTHON_EXECUTABLE} -m pytest --color=yes ${CMAKE_CURRENT_SOURCE_DIR} -k test_sanity)
  add_test(NAME verilog-custom-tests
    COMMAND  ${CMAKE_COMMAND} -E env SYSTEMC_CLANG=${PROJECT_SOURCE_DIR} ${PYTHON_EXECUTABLE} -m pytest --color=yes ${CMAKE_CURRENT_SOURCE_DIR} -k test_custom)
endif()

