#
# Credit should be awarded to sources.  
# 
# Credits:
# https://github.com/andreasfertig/cppinsights/blob/master/CMakeLists.txt

cmake_minimum_required(VERSION 3.3)
# Fix behavior of CMAKE_CXX_STANDARD when targeting macOS.
if (POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif ()

# Project name
project(systemc-clang)

# Use C++ standard version to c++14
set(CMAKE_CXX_STANDARD 14)

# Always keep it on
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#  Make verbose on?
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

# Maya
# Pass the flagg -DCMAKE_BUILD_TYPE=RelWithDebInfo
# set(CMAKE_BUILD_TYPE RelWithDebInfo)
# end Maya

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

#set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


# SystemC-Clang versions
set(SCC_MAJOR_VERSION 1)
set(SCC_MINOR_VERSION ${SCC_MAJOR_VERSION}.1)

# Provide path for scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# 
# This is the library to use for unit tests
#
# Prepare "Catch" library for other executables
set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals)

add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

# Include ctest and catch2
include(CTest)
include(Catch)


# Required packages
# Include the these flags whenever you compile.
add_definitions (-D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS)

# Set the path for where LLVM/clang is installed.
set( LLVM_INSTALL_DIR $ENV{LLVM_INSTALL_DIR} )

# The package requires Clang_DIR.
set( CLANG_DIR ${Clang_DIR} )

# Set the SystemC path
set( SYSTEMC_DIR $ENV{SYSTEMC} )

# Set extra include dir
set( EXTRA_INCLUDE_DIR $ENV{EXTRA_INCLUDE_DIR} )

# Default plugin status 
if ("${XLAT}" STREQUAL "")
  set( PLUGIN_XLAT OFF )
else()
  set( PLUGIN_XLAT ${XLAT})
endif()


# Keep tests off by default
if ("${ENABLE_TESTS}" STREQUAL "")
  set( TESTS_ON OFF )
else()
  set( TESTS_ON ${ENABLE_TESTS})
  # Check that SYSTEMC_DIR is defined
  if ("${SYSTEMC_DIR}" STREQUAL "")
    message(FATAL_ERROR "SYSTEMC_DIR should specify path to SystemC. ${HAD_ERROR}")
  endif()

endif()

# Keep python tests off by default
if ("${ENABLE_VERILOG_TESTS}" STREQUAL "")
  set( V_TESTS_ON OFF )
else()
  set( V_TESTS_ON ${ENABLE_VERILOG_TESTS} )
  if (V_TESTS_ON)
    find_package(PythonInterp 3 REQUIRED)
  endif()
  if (V_TESTS_ON)
    if ("${XLAT}" STREQUAL "")
      set( PLUGIN_XLAT ON )
      message(WARNING "XLAT enabled. XLAT must be turned on in order to run Verilog tests")
    endif()
  endif()
endif()

# Test package

set (search_paths
  ${LLVM_INSTALL_DIR}
  ${LLVM_INSTALL_DIR}/lib/cmake
  ${LLVM_INSTALL_DIR}/lib/cmake/llvm
  ${LLVM_INSTALL_DIR}/lib/cmake/clang
  ${LLVM_INSTALL_DIR}/share/clang/cmake/
  ${LLVM_INSTALL_DIR}/share/llvm/cmake/
  ${CLANG_DIR}
  ${CLANG_DIR}/lib/cmake
  ${CLANG_DIR}/lib/cmake/llvm
  ${CLANG_DIR}/lib/cmake/clang
  ${CLANG_DIR}/share/clang/cmake/
  ${CLANG_DIR}/share/llvm/cmake/

  )

# If the environment variable LLVM_INSTALL_DIR is not set, then try to look for the packages.
# If they are found, then use the path for LLVM_INSTALL_DIR produced from the package.
if ("${LLVM_INSTALL_DIR}" STREQUAL "")

  message(STATUS "Attempting to find package for Clang.")
  #find_package(LLVM REQUIRED CONFIG
  #             PATHS ${search_paths}
  #             NO_DEFAULT_PATH)
  #
  find_package(Clang REQUIRED CONFIG
    PATHS ${search_paths}
    NO_DEFAULT_PATH)

  #message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
  #message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
  message(STATUS "Found CLang ${CLANG_PACKAGE_VERSION}")
  message(STATUS "Using ClangConfig.cmake in: ${CLANG_DIR}")

  # Reset the LLVM_INSTALL_DIR environment variable once the package is found.
  set( LLVM_INSTALL_DIR ${CLANG_DIR} )
endif()


option (USE_SILENT_BUILD
  "Silent as many warnings as possible." OFF
  )

if(PLUGIN_XLAT)
  set( ROOT_CPP driver-xlat.cpp )
else() 
  set( ROOT_CPP driver-tooling.cpp )
endif()

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

# Check whether the compiler is AppleClang, which requires special headers for the tests
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
  set(IS_APPLECLANG "1")
else()
  set(IS_APPLECLANG "0")
endif()


# Find llvm-config to setup the remaining paths.
# switch to the variable named VARNAME.
#
# Example usage: llvm_config(LLVM_CXXFLAGS "--cxxflags")
function(llvm_config VARNAME switch)
  set(CONFIG_COMMAND ${LLVM_CONFIG_EXE} ${switch})

  execute_process(
    COMMAND ${CONFIG_COMMAND} 
    RESULT_VARIABLE HAD_ERROR
    OUTPUT_VARIABLE CONFIG_OUTPUT
    )

  if (HAD_ERROR)
    string(REPLACE ";" " " CONFIG_COMMAND_STR "${CONFIG_COMMAND}")
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
    # Replace llvm-config's c++11 generation
    #string(REGEX REPLACE "-std=c++11" "" CONFIG_OUTPUT "${CONFIG_OUTPUT}")
  endif()

  # make result available outside
  set(${VARNAME} ${CONFIG_OUTPUT} PARENT_SCOPE)

  # Find IWYU
#
  # find_program(iwyu_path NAMES include-what-you-use iwyu)
# if(NOT iwyu_path)
  # message(FATAL_ERROR "Could not find the program include-what-you-use")
# endif()
#

  # cleanup
  unset(CONFIG_COMMAND)
endfunction(llvm_config)


# Find llvm-config
find_program(LLVM_CONFIG_EXE
  NAMES llvm-config
  HINTS ${LLVM_INSTALL_DIR}/bin/ 
  DOC "llvm-config executable"
  PATHS ${LLVM_INSTALL_DIR}/bin/ NO_DEFAULT_PATH)

if(NOT LLVM_CONFIG_EXE)
  message(FATAL_ERROR "llvm-config not found -- ${LLVM_CONFIG_EXE} ")
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
llvm_config(LLVM_LIBS "--libs")
llvm_config(LLVM_LD_FLAGS "--ldflags")
llvm_config(CLANG_VERSION "--version")

#
# There was an issue with the system-libs not being added. Something seems to have changed from version 7+ 
# with respect to llvm-config --libs. 
#
# https://github.com/halide/Halide/issues/1112
llvm_config(LLVM_SYSTEM_LIBS "--system-libs")

set( CMAKE_CXX_FLAGS "${LLVM_CXX_FLAGS}" )

if (USE_SILENT_BUILD)
  set (CMAKE_CXX_FLAGS
    "-DUSE_SILENT_BUILD=on ${CMAKE_CXX_FLAGS}"
    )
endif(USE_SILENT_BUILD)


set( LLVM_LD_FLAGS "${LLVM_LD_FLAGS}" )

set( LLVM_LIBS ${LLVM_LIBS} )

set(CLANG_LIBS
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

set(CMAKE_MODULE_LINKER_FLAGS
  ${LLVM_LD_FLAGS}
  )

set( TARGET "systemc-clang" )

# catch2 may have headers colliding with other local files, so we can use -iquote instead of -I to include files in src folder

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -iquote ${PROJECT_SOURCE_DIR}/src")
include_directories(
  "${LLVM_INSTALL_DIR}/include"
  plugins/xlat
  "${CATCH_INCLUDE_DIR}"
  )

if(NOT "${EXTRA_INCLUDE_DIR}" STREQUAL "")
  include_directories( "${EXTRA_INCLUDE_DIR}")
endif()


link_directories( ${LLVM_INSTALL_DIR}/lib )

add_subdirectory(src)
add_subdirectory(plugins)

if( TESTS_ON )
  add_subdirectory(tests)
  enable_testing()
endif()

set( SRC_LIB libsystemc-clang libxlat )

add_executable(
  ${TARGET}
  ${ROOT_CPP} 
  )

target_link_libraries(${TARGET} ${SRC_LIB}) #nlohmann_json::nlohmann_json)

foreach (clang_lib ${CLANG_LIBS})
  target_link_libraries( ${TARGET} ${clang_lib} )
endforeach()

foreach (llvm_lib ${LLVM_LIBS})
  target_link_libraries( ${TARGET} ${llvm_lib} )
endforeach()

foreach (ld_flag ${LLVM_LD_FLAGS} ${LLVM_SYSTEM_LIBS})
  target_link_libraries( ${TARGET} ${ld_flag} )
endforeach()

#set_property(TARGET "systemc-clang" PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})

# Print the summary of the build. 
message(STATUS "")
message(STATUS "[ Build summary ]")
message(STATUS "SystemC-Clang         : ${SCC_MINOR_VERSION}")
message(STATUS "")
message(STATUS "[ Plugins       ]")
message(STATUS "Plugin Xlat           : ${PLUGIN_XLAT}")
message(STATUS "")
message(STATUS "[ Tests         ]")
message(STATUS "Build tests           : ${TESTS_ON}")
message(STATUS "Build Verilog tests   : ${V_TESTS_ON}")
message(STATUS "")
message(STATUS "[ Environment ]")
message(STATUS "CMAKE_GENERATOR       : ${CMAKE_GENERATOR}")
message(STATUS "Compiler ID           : ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Compiler version      : ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Compiler path         : ${CMAKE_CXX_COMPILER}")
message(STATUS "")
message(STATUS "clang version         : ${CLANG_VERSION}")
message(STATUS "clang path            : ${LLVM_INSTALL_DIR}")
message(STATUS "llvm-config           : ${LLVM_CONFIG_EXE}")
message(STATUS "")
message(STATUS "SystemC path          : ${SYSTEMC_DIR}")
message(STATUS "")
message(STATUS "Install path          : ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_SOURCE_DIR      : ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR      : ${CMAKE_BINARY_DIR}")
message(STATUS "")
message(STATUS "Python found          : ${PYTHONINTERP_FOUND}")
message(STATUS "Python interpreter    : ${PYTHON_EXECUTABLE}")
message(STATUS "")
message(STATUS "[ Git info.     ]")
message(STATUS "Git repo url          : ${GIT_REPO_URL}")
message(STATUS "Git commit hash       : ${GIT_COMMIT_HASH}")
message(STATUS "")
