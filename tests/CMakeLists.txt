# Test CMakeFile

set(UNIT_TEST_LIST 
  catch-test 
  catch-test2 
  model.test 
  )
 
include(CTest)

foreach(NAME IN LISTS UNIT_TEST_LIST)
  list(APPEND UNIT_TEST_SOURCE_LIST
    ${NAME}.cpp)
  set( TEST_SOURCE_NAME ${NAME})
  set(TARGET_NAME ${NAME})
  add_executable(${TARGET_NAME} main.cpp ${TEST_SOURCE_NAME})
  target_link_libraries(${TARGET_NAME} PUBLIC libsystemc-clang ${CLANG_LIBS} ${LLVM_LIBS} ${LLVM_SYSTEM_LIBS})
  target_include_directories(${TARGET_NAME} PUBLIC ../externals/catch2/ ../src/)
  add_test( NAME ${TARGET_NAME} COMMAND ${TARGET_NAME} )
endforeach()

