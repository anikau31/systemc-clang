#
# Credit should be awarded to sources.  
# 
# Credits:
# https://github.com/andreasfertig/cppinsights/blob/master/CMakeLists.txt

cmake_minimum_required(VERSION 2.6)

# Project name
project(systemc-clang)

# Use C++ standard version to c++14
set(CMAKE_CXX_STANDARD 14)

# Always keep it on
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#  Make verbose on?
#set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

# SystemC-Clang versions
set(SCC_MAJOR_VERSION 1)
set(SCC_MINOR_VERSION ${SCC_MAJOR_VERSION}.1)

set(SCC_LLVM_CONFIG "llvm-config" CACHE STRING "LLVM config executable to use")

# Required packages
# Include the these flags whenever you compile.
add_definitions (-D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS)

option (USE_SILENT_BUILD
  "Silent as many warnings as possible." OFF
  )

set (ROOT_CPP
  driver-tooling.cpp
  )

# Set the path for where LLVM/clang is installed.
set (LLVM_INSTALL_DIR
  $ENV{LLVM_INSTALL_DIR}
  )

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(IS_GNU On)
else()
    set(IS_GNU Off)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(IS_CLANG On)
else()
    set(IS_CLANG Off)
endif()


# Find llvm-config to setup the remaining paths.
# switch to the variable named VARNAME.
#
# Example usage: llvm_config(LLVM_CXXFLAGS "--cxxflags")
function(llvm_config VARNAME switch)
  set(CONFIG_COMMAND "${LLVM_CONFIG_EXE}" "${switch}")

  execute_process(
    COMMAND ${CONFIG_COMMAND} ${LIB_TYPE}
    RESULT_VARIABLE HAD_ERROR
    OUTPUT_VARIABLE CONFIG_OUTPUT
    )
  if (HAD_ERROR)
    string(REPLACE ";" " " CONFIG_COMMAND_STR "${CONFIG_COMMAND}")
    message(STATUS "${CONFIG_COMMAND_STR}")
    message(FATAL_ERROR "llvm-config failed with status ${HAD_ERROR}")
  endif()

  # replace linebreaks with semicolon
  string(REGEX REPLACE
    "[ \t]*[\r\n]+[ \t]*" ";"
    CONFIG_OUTPUT ${CONFIG_OUTPUT})

  # make all includes system include to prevent the compiler to warn about issues in LLVM/Clang
  if(NOT WIN32)
    string(REGEX REPLACE "-I" "-isystem" CONFIG_OUTPUT "${CONFIG_OUTPUT}")
  else()
    if(IS_MSVC_CL)
      # See https://devblogs.microsoft.com/cppblog/broken-warnings-theory/
      string(REGEX REPLACE "-I" "/external:I" CONFIG_OUTPUT "${CONFIG_OUTPUT}")
    endif()
  endif()

  # remove certain options clang doesn't like
  if(IS_CLANG)
    string(REGEX REPLACE "-Wl,--no-keep-files-mapped" "" CONFIG_OUTPUT "${CONFIG_OUTPUT}")
    string(REGEX REPLACE "-Wl,--no-map-whole-files" "" CONFIG_OUTPUT "${CONFIG_OUTPUT}")
    string(REGEX REPLACE "-fuse-ld=gold" "" CONFIG_OUTPUT "${CONFIG_OUTPUT}")
  elseif(IS_GNU)
    string(REGEX REPLACE "-Wcovered-switch-default" "" CONFIG_OUTPUT "${CONFIG_OUTPUT}")
    string(REGEX REPLACE "-Wstring-conversion" "" CONFIG_OUTPUT "${CONFIG_OUTPUT}")
    string(REGEX REPLACE "-Werror=unguarded-availability-new" "" CONFIG_OUTPUT "${CONFIG_OUTPUT}")
    string(REGEX REPLACE "-Wno-unknown-warning-option" "" CONFIG_OUTPUT "${CONFIG_OUTPUT}")
    string(REGEX REPLACE "-Wno-unused-command-line-argument" "" CONFIG_OUTPUT "${CONFIG_OUTPUT}")
  endif()

  # make result available outside
  set(${VARNAME} ${CONFIG_OUTPUT} PARENT_SCOPE)

  # Optionally output the configured value
  message(STATUS "llvm_config(${VARNAME})=>${CONFIG_OUTPUT}")

  # cleanup
  unset(CONFIG_COMMAND)
endfunction(llvm_config)

# Find llvm-config
  find_program(LLVM_CONFIG_EXE
    NAMES llvm-config
    DOC "llvm-config executable"
    PATHS ${LLVM_INSTALL_DIR}/bin/ NO_DEFAULT_PATH)

  if(NOT LLVM_CONFIG_EXE)
    message(FATAL_ERROR "llvm-config not found -- ${LLVM_CONFIG_EXE}: ${SCC_LLVM_CONFIG}")
  endif()

  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
  execute_process(
    COMMAND git log -1 --format=%H
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  execute_process(
    COMMAND git config --get remote.origin.url
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_REPO_URL
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

else()
  set(GIT_REPO_URL "")
  set(GIT_COMMIT_HASH "")
endif()


  #################################################################################

  # Set the corresponding LLVM flags using llvm-config
  llvm_config(LLVM_CXX_FLAGS "--cxxflags")
  llvm_config(LLVM_LD_FLAGS "--ldflags")
  llvm_config(LLVM_LIBS "--libs")

set (CMAKE_CXX_FLAGS
   "${LLVM_CXX_FLAGS}"
  )

if (USE_SILENT_BUILD)
  set (CMAKE_CXX_FLAGS
    "-DUSE_SILENT_BUILD=on ${CMAKE_CXX_FLAGS}"
    )
endif(USE_SILENT_BUILD)


set (LLVM_LD_FLAGS
  "${LLVM_LD_FLAGS} -lpthread"
  )

set (LLVM_LIBS
  ${LLVM_LIBS}
  )

set (CLANG_LIBS
  clangTooling
  clangFrontendTool
  clangFrontend
  clangDriver
  clangSerialization
  clangCodeGen
  clangParse
  clangSema
  clangStaticAnalyzerFrontend
  clangStaticAnalyzerCheckers
  clangStaticAnalyzerCore
  clangAnalysis
  clangARCMigrate
  clangRewriteFrontend
  clangRewrite
  clangEdit
  clangAST
  clangLex
  clangBasic
  clangASTMatchers
  )


set (CMAKE_MODULE_LINKER_FLAGS
  ${LLVM_LD_FLAGS}
  )

set (TARGET
  "systemc-clang"
  )


include_directories(
  "${LLVM_INSTALL_DIR}/include"
  src
 )


link_directories(
  ${LLVM_INSTALL_DIR}/lib
  )

add_subdirectory(src)
add_subdirectory(tests)


set( SRC_LIB libsystemc-clang )

add_executable(
  ${TARGET}
  ${ROOT_CPP} )

target_link_libraries(${TARGET} ${SRC_LIB}) #nlohmann_json::nlohmann_json)

foreach (clang_lib ${CLANG_LIBS})
  target_link_libraries( ${TARGET} ${clang_lib} )
endforeach()

foreach (ld_flag ${LLVM_LD_FLAGS})
  target_link_libraries( ${TARGET} ${ld_flag} )
endforeach()

foreach (llvm_lib ${LLVM_LIBS})
  target_link_libraries( ${TARGET} ${llvm_lib} )
endforeach()



# Print the summary of the build. 
message(STATUS "")
message(STATUS "[ Build summary ]")
message(STATUS "SystemC-Clang         : ${SCC_MINOR_VERSION}")
message(STATUS "CMAKE_GENERATOR       : ${CMAKE_GENERATOR}")
message(STATUS "Compiler ID           : ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Compiler version      : ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Compiler path         : ${CMAKE_CXX_COMPILER}")
message(STATUS "llvm-config           : ${LLVM_CONFIG_EXE}")
message(STATUS "Install path          : ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_SOURCE_DIR      : ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR      : ${CMAKE_BINARY_DIR}")
message(STATUS "Git repo url          : ${GIT_REPO_URL}")
message(STATUS "Git commit hash       : ${GIT_COMMIT_HASH}")
message(STATUS "")
