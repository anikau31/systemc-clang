# Test CMakeFile

configure_file( ClangArgs.h.in ClangArgs.h)

# Test files so far are t#.cpp
# Don't use globbing as it doesn't recognize recompiles.
# file(GLOB TEST_FILES "t[0-9].cpp")

# copy the directory of data to test build
file(COPY data DESTINATION ${CMAKE_BINARY_DIR}/tests/ )

# copy some of the examples that are necessary for testing
# be careful about the filename as they may collide 
file(COPY ../examples/llnl-examples DESTINATION ${CMAKE_BINARY_DIR}/tests/data )

set( UNIT_TEST_LIST 
  # t1
  # t2
  # t3
  # sreg-test
  # t4-matchers
  t5-template-matching
  # subtree-matcher-test
  )

include(CTest)

# catch2 may have headers colliding with other local files, 
# so we can use -iquote instead of -I to include files in src folder

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -iquote ${PROJECT_SOURCE_DIR}/src")
foreach(NAME IN LISTS UNIT_TEST_LIST)
  list(APPEND UNIT_TEST_SOURCE_LIST
    ${NAME}.cpp)
  set(TEST_SOURCE_NAME ${NAME})
  set(TARGET_NAME ${NAME})
  add_executable(${TARGET_NAME} main.cpp ${TEST_SOURCE_NAME} )
    
  target_link_libraries(${TARGET_NAME} PUBLIC libsystemc-clang ${CLANG_LIBS} ${LLVM_LIBS} ${LLVM_SYSTEM_LIBS})
  target_include_directories(${TARGET_NAME} PUBLIC ../externals/catch2/ PUBLIC ../tests/ PUBLIC ${CMAKE_BINARY_DIR}/tests/ )
  add_test( NAME ${TARGET_NAME} COMMAND ${TARGET_NAME} 
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests/data/ )
endforeach()

# for pytest tests
if(PY_TESTS_ON)
  add_test(NAME python-tests
    COMMAND ${PYTHON_EXECUTABLE} -m pytest --color=yes ${CMAKE_CURRENT_SOURCE_DIR}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} )
endif()

