# Test CMakeFile

set (TEST_LIST 
  # Thread parsing 
  simple-thread.cpp 
  run-cfg-test.cpp
  )

set(SCC_LIBS libsystemc-clang)

# catch2 may have headers colliding with other local files, 
# so we can use -iquote instead of -I to include files in src folder

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -iquote ${PROJECT_SOURCE_DIR}/src")

# This is to link with SystemC clang
foreach(NAME IN LISTS TEST_LIST)
  list(APPEND UNIT_TEST_SOURCE_LIST
    ${NAME}.cpp)
  set(TEST_SOURCE_NAME ${NAME})
  set(TARGET_NAME ${NAME})
  add_executable(${TARGET_NAME} ${PROJECT_SOURCE_DIR}/${TEST_DIR}/main.cpp ${TEST_SOURCE_NAME} )
    
  target_link_libraries(${TARGET_NAME} 
    PUBLIC ${SCC_LIBS} ${CLANG_LIBS} ${LLVM_LIBS} ${LLVM_SYSTEM_LIBS})
 

  target_include_directories(${TARGET_NAME} 
    PUBLIC ${PROJECT_SOURCE_DIR}/externals/catch2/ 
    PUBLIC ${PROJECT_SOURCE_DIR}/${TEST_DIR}/ 
    PUBLIC ${CMAKE_BINARY_DIR}/${TEST_DIR}/ 
    )

  add_test( NAME ${TARGET_NAME} COMMAND ${TARGET_NAME} 
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${TEST_DIR}/data/ )
endforeach()


