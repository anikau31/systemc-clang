# Test CMakeFile

# Expand extra include directories for tests
set(xtra_inc_dir_test $ENV{EXTRA_INCLUDE_DIR})
set(EXTRA_INCLUDE_DIR_EXPAND "")
# Prepending "-I" and appending ","
foreach(d ${xtra_inc_dir_test})
  string(CONCAT EXTRA_INCLUDE_DIR_EXPAND ${EXTRA_INCLUDE_DIR_EXPAND} "\n    \"-I${d}\",")
endforeach()
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

 # add_subdirectory(unit)
 # add_subdirectory(matchers)
 # add_subdirectory(parsing)
 add_subdirectory(cfg)

# # Plugins
# add_subdirectory(hdl)


include(CTest)

# catch2 may have headers colliding with other local files, 
# so we can use -iquote instead of -I to include files in src folder

# for pytest tests
if(V_TESTS_ON)
  # integrate into ctest
  add_test(NAME hcode-hmethod-call
    COMMAND  ${CMAKE_COMMAND} -E env SYSTEMC_CLANG=${PROJECT_SOURCE_DIR} ${PYTHON_EXECUTABLE} -B -m pytest -v --color=yes ${CMAKE_CURRENT_SOURCE_DIR} -k hmethod_call)
  add_test(NAME hcode-unimpl
    COMMAND  ${CMAKE_COMMAND} -E env SYSTEMC_CLANG=${PROJECT_SOURCE_DIR} ${PYTHON_EXECUTABLE} -B -m pytest -v --color=yes ${CMAKE_CURRENT_SOURCE_DIR} -k unimpl)
  add_test(NAME verilog-syntax-error
    COMMAND  ${CMAKE_COMMAND} -E env SYSTEMC_CLANG=${PROJECT_SOURCE_DIR} ${PYTHON_EXECUTABLE} -B -m pytest -v --color=yes ${CMAKE_CURRENT_SOURCE_DIR} -k syntax_error)
  add_test(NAME verilog-generate-file
    COMMAND  ${CMAKE_COMMAND} -E env SYSTEMC_CLANG=${PROJECT_SOURCE_DIR} ${PYTHON_EXECUTABLE} -B -m pytest -v --color=yes ${CMAKE_CURRENT_SOURCE_DIR} -k generate_file)
  add_test(NAME verilog-translation-add-hcode-match
    COMMAND  ${CMAKE_COMMAND} -E env SYSTEMC_CLANG=${PROJECT_SOURCE_DIR} ${PYTHON_EXECUTABLE} -B -m pytest -v --color=yes ${CMAKE_CURRENT_SOURCE_DIR} -k translation[add-hcode )
  add_test(NAME verilog-translation-add
    COMMAND  ${CMAKE_COMMAND} -E env SYSTEMC_CLANG=${PROJECT_SOURCE_DIR} ${PYTHON_EXECUTABLE} -B -m pytest -v --color=yes ${CMAKE_CURRENT_SOURCE_DIR} -k translation[add- )
  add_test(NAME verilog-translation-z3test
    COMMAND  ${CMAKE_COMMAND} -E env SYSTEMC_CLANG=${PROJECT_SOURCE_DIR} ${PYTHON_EXECUTABLE} -B -m pytest -v --color=yes ${CMAKE_CURRENT_SOURCE_DIR} -k translation[z3test- )
  add_test(NAME verilog-translation-moving-average
    COMMAND  ${CMAKE_COMMAND} -E env SYSTEMC_CLANG=${PROJECT_SOURCE_DIR} ${PYTHON_EXECUTABLE} -B -m pytest -v --color=yes ${CMAKE_CURRENT_SOURCE_DIR} -k translation[moving-average- )
  add_test(NAME verilog-translation-while-iscs
    COMMAND  ${CMAKE_COMMAND} -E env SYSTEMC_CLANG=${PROJECT_SOURCE_DIR} ${PYTHON_EXECUTABLE} -B -m pytest -v --color=yes ${CMAKE_CURRENT_SOURCE_DIR} -k translation[test_while_iscs- )
  # add_test(NAME verilog-translation-binary-iscs
  #   COMMAND  ${CMAKE_COMMAND} -E env SYSTEMC_CLANG=${PROJECT_SOURCE_DIR} ${PYTHON_EXECUTABLE} -B -m pytest -v --color=yes ${CMAKE_CURRENT_SOURCE_DIR} -k translation[test_binary_iscs- )
  add_test(NAME verilog-translation-break-iscs
    COMMAND  ${CMAKE_COMMAND} -E env SYSTEMC_CLANG=${PROJECT_SOURCE_DIR} ${PYTHON_EXECUTABLE} -B -m pytest -v --color=yes ${CMAKE_CURRENT_SOURCE_DIR} -k translation[test_break_iscs- )
  add_test(NAME verilog-translation-for-iscs
    COMMAND  ${CMAKE_COMMAND} -E env SYSTEMC_CLANG=${PROJECT_SOURCE_DIR} ${PYTHON_EXECUTABLE} -B -m pytest -v --color=yes ${CMAKE_CURRENT_SOURCE_DIR} -k translation[test_for_iscs- )
  add_test(NAME verilog-translation-child-module-iscs
    COMMAND  ${CMAKE_COMMAND} -E env SYSTEMC_CLANG=${PROJECT_SOURCE_DIR} ${PYTHON_EXECUTABLE} -B -m pytest -v --color=yes ${CMAKE_CURRENT_SOURCE_DIR} -k translation[test_child_module_iscs- )
  # add_test(NAME verilog-translation-dowhile-iscs
  #   COMMAND  ${CMAKE_COMMAND} -E env SYSTEMC_CLANG=${PROJECT_SOURCE_DIR} ${PYTHON_EXECUTABLE} -B -m pytest -v --color=yes ${CMAKE_CURRENT_SOURCE_DIR} -k translation[test_dowhile_iscs- )
  # add_test(NAME verilog-translation-for-fcall-fail-iscs
  #   COMMAND  ${CMAKE_COMMAND} -E env SYSTEMC_CLANG=${PROJECT_SOURCE_DIR} ${PYTHON_EXECUTABLE} -B -m pytest -v --color=yes ${CMAKE_CURRENT_SOURCE_DIR} -k translation[test_for_fcall_fail_iscs- )
  # add_test(NAME verilog-translation-forloop-other-types-iscs
  #   COMMAND  ${CMAKE_COMMAND} -E env SYSTEMC_CLANG=${PROJECT_SOURCE_DIR} ${PYTHON_EXECUTABLE} -B -m pytest -v --color=yes ${CMAKE_CURRENT_SOURCE_DIR} -k translation[test_forloop_other_types_iscs- )
  # add_test(NAME verilog-translation-virtual1-iscs
  #   COMMAND  ${CMAKE_COMMAND} -E env SYSTEMC_CLANG=${PROJECT_SOURCE_DIR} ${PYTHON_EXECUTABLE} -B -m pytest -v --color=yes ${CMAKE_CURRENT_SOURCE_DIR} -k translation[test_virtual1_iscs- )
  # add_test(NAME verilog-translation-vname-iscs
  #   COMMAND  ${CMAKE_COMMAND} -E env SYSTEMC_CLANG=${PROJECT_SOURCE_DIR} ${PYTHON_EXECUTABLE} -B -m pytest -v --color=yes ${CMAKE_CURRENT_SOURCE_DIR} -k translation[test_vname_iscs- )
  add_test(NAME verilog-translation-while-const-iscs
    COMMAND  ${CMAKE_COMMAND} -E env SYSTEMC_CLANG=${PROJECT_SOURCE_DIR} ${PYTHON_EXECUTABLE} -B -m pytest -v --color=yes ${CMAKE_CURRENT_SOURCE_DIR} -k translation[test_while_const_iscs- )
endif()

