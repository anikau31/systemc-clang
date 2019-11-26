# Test CMakeFile

set(UNIT_TEST_LIST 
  catch-test 
  catch-test2 
  model.test 
  build-ast-code
  sc-clang-test
  )
 
include(CTest)

# catch2 may have headers colliding with other local files, so we can use -iquote instead of -I to include files in src folder
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -iquote ${PROJECT_SOURCE_DIR}/src")
foreach(NAME IN LISTS UNIT_TEST_LIST)
  list(APPEND UNIT_TEST_SOURCE_LIST
    ${NAME}.cpp)
  set( TEST_SOURCE_NAME ${NAME})
  set(TARGET_NAME ${NAME})
  add_executable(${TARGET_NAME} main.cpp ${TEST_SOURCE_NAME})
  target_link_libraries(${TARGET_NAME} PUBLIC libsystemc-clang ${CLANG_LIBS} ${LLVM_LIBS} ${LLVM_SYSTEM_LIBS})
  target_include_directories(${TARGET_NAME} PUBLIC ../externals/catch2/)
  add_test( NAME ${TARGET_NAME} COMMAND ${TARGET_NAME} )
endforeach()


