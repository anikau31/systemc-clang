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


# Use C++ standard version to c++17
set(CMAKE_CXX_STANDARD 17)

# Always keep it on
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#  Make verbose on?
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

#set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


# SystemC-Clang versions
set(SCC_MAJOR_VERSION 2)
set(SCC_MINOR_VERSION ${SCC_MAJOR_VERSION}.2)

# Provide path for scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

 
##################################################
# Catch2
##################################################
# This is the library to use for unit tests
#
# Prepare "Catch" library for other executables
set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals)
# Disable throwing exceptions in catch2
#add_definitions (-DCATCH_CONFIG_DISABLE_EXCEPTIONS)

add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

# Include ctest and catch2
include(CTest)
include(Catch)

option(BUILD_DOC "Set this option to build with documentation targets" OFF)

if (BUILD_DOC)
##################################################
# Sphinx
##################################################
include(FindSphinx)

set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/docs/source/)
set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/docs/)

add_custom_target(sphinx
                  COMMAND
                  ${SPHINX_EXECUTABLE} -M html
                  ${SPHINX_SOURCE} ${SPHINX_BUILD}
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                  COMMENT "Generating documentation with Sphinx")


##################################################
# Doxygen
##################################################
# look for Doxygen package
find_package(Doxygen)

if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/source//Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    # Note: do not put "ALL" - this builds docs together with application EVERY TIME!
    add_custom_target( doxygen
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
  message(FATAL_ERROR "FATAL: DOXYGEN not found.")

endif (DOXYGEN_FOUND)

if (DOXYGEN_FOUND AND SPHINX_EXECUTABLE)
add_custom_target(docs  
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
    COMMAND
              ${SPHINX_EXECUTABLE} -M html
              ${SPHINX_SOURCE} ${SPHINX_BUILD}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating Doxygen and Sphinx documentation"
    VERBATIM )
endif()

endif (BUILD_DOC)
##################################################
# Required packages
# Include the these flags whenever you compile.
add_definitions(-D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS)

# Disable exceptions in nlohmann::json.
add_definitions(-DJSON_EXCEPTION)

# Set the path for where LLVM/clang is installed.
set( LLVM_INSTALL_DIR $ENV{LLVM_INSTALL_DIR} )

# The package requires Clang_DIR.
set( CLANG_DIR ${Clang_DIR} )

# Set the SystemC path
set( SYSTEMC_DIR $ENV{SYSTEMC} )

# Set extra include dir
set( EXTRA_INCLUDE_DIR $ENV{EXTRA_INCLUDE_DIR} )

# Default plugin status 
option( HDL "Compile HDL plugin" OFF )

set( PLUGIN_HDL ${HDL})

# if (("${HDL}" STREQUAL "ON") OR ("${HDL}" STREQUAL "on"))
  # set( PLUGIN_HDL ON)
# endif()
#
# Disallow in-source builds.
# Source: https://github.com/opencv/opencv/pull/8582/files<Paste>
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}") 
  message(FATAL_ERROR "
  FATAL: In-source builds are prohibited.
         Create a separate directory for build files.")
endif() 

# Keep tests off by default
if ("${ENABLE_TESTS}" STREQUAL "")
  set( TESTS_ON OFF )
else()
  set( TESTS_ON ${ENABLE_TESTS})
  # Check that SYSTEMC_DIR is defined
  if ("${SYSTEMC_DIR}" STREQUAL "")
    message(FATAL_ERROR "
    FATAL: SYSTEMC_DIR not set.
           Specify the path to SystemC in SYSTEMC_DIR. ${HAD_ERROR}")
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
    if ("${HDL}" STREQUAL "")
      set( PLUGIN_HDL ON )
      message(WARNING "
			WARNING: HDL enabled. 
						   HDL must be turned on in order to run Verilog tests")
    endif()
  endif()
endif()

# Hardware flow relies on HDL plugin to generate SystemVerilog code
if ("${ENABLE_HARDWARE_FLOW}" STREQUAL "")
  set( HW_FLOW_ON OFF )
else()
  set( HW_FLOW_ON ${ENABLE_HARDWARE_FLOW} )
  if (HW_FLOW_ON)
    find_package(PythonInterp 3 REQUIRED)
  endif()
  if("${HDL}" STREQUAL "")
    set( PLUGIN_HDL ON )
      message(WARNING "
			WARNING: HDL enabled. 
						   HDL must be turned on in order to run hardware flow")
  endif()
endif()


# Use ccache
if (("${USE_CCACHE}" STREQUAL "ON") OR ("${USE_CCACHE}" STREQUAL "on"))
  set(USE_CCACHE ON)
else()
  set(USE_CCACHE OFF)
endif()

if(USE_CCACHE) # set by build.sh
  find_program(CCACHE_PROGRAM ccache)
  if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    set(CMAKE_C_COMPILER_LAUNCHER   ${CCACHE_PROGRAM})
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

  message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
  message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
  message(STATUS "Found CLang ${CLANG_PACKAGE_VERSION}")
  message(STATUS "Using ClangConfig.cmake in: ${CLANG_DIR}")

  # Reset the LLVM_INSTALL_DIR environment variable once the package is found.
  set( LLVM_INSTALL_DIR ${CLANG_DIR} )
endif()


option (USE_SILENT_BUILD
  "Silent as many warnings as possible." OFF
  )

if(PLUGIN_HDL)
  set( ROOT_CPP driver-hdl.cpp )
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
  set(APPLECLANG_EXTRA_INC \"-I${LLVM_INSTALL_DIR}/include/c++/v1/\",)
else()
  set(IS_APPLECLANG "0")
  set(APPLECLANG_EXTRA_INC "")
endif()

# Module to find LLVM/Clang and llvm-config.
include(FindLLVM)
#################################################################################

# Set the corresponding LLVM flags using llvm-config
llvm_config(LLVM_CXX_FLAGS "--cxxflags" "")
llvm_config(LLVM_LIBS "--libs" "")
llvm_config(LLVM_LD_FLAGS "--ldflags" "")
llvm_config(CLANG_VERSION "--version" "")

#
# There was an issue with the system-libs not being added. Something seems to have changed from version 7+ 
# with respect to llvm-config --libs. 
#
# https://github.com/halide/Halide/issues/1112
llvm_config(LLVM_SYSTEM_LIBS "--system-libs" "")
message(STATUS "LLVM_SYSTEM_LIBS: ${LLVM_SYSTEM_LIBS}")

#set( CMAKE_CXX_FLAGS "${LLVM_CXX_FLAGS}" )
set( CMAKE_CXX_FLAGS "${LLVM_CXX_FLAGS}  -fvisibility=hidden -fvisibility-inlines-hidden" )

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
  clangStaticAnalyzerCore
  clangAnalysis
  clangEdit
  clangAST
  clangLex
  clangBasic
  clangASTMatchers
  #clangStaticAnalyzerCheckers
  #clangARCMigrate
  #clangRewriteFrontend
  #clangRewrite

  )

set(CMAKE_MODULE_LINKER_FLAGS
  ${LLVM_LD_FLAGS}
  )

set( TARGET "systemc-clang" )

# catch2 may have headers colliding with other local files, so we can use -iquote instead of -I to include files in src folder

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -iquote ${PROJECT_SOURCE_DIR}/src")
include_directories(
  "${LLVM_INSTALL_DIR}/include"
  "${LLVM_INSTALL_DIR}/../clang/include"
  "${LLVM_INSTALL_DIR}/tools/clang/include"
  "${CATCH_INCLUDE_DIR}"
  "src/"
  "src/matchers"
  "src/model"
  "src/cfg"
  "src/utils"
  "src/SCuitable"
  )

set( SRC_LIB libsystemc-clang )

if(NOT "${EXTRA_INCLUDE_DIR}" STREQUAL "")
  include_directories( "${EXTRA_INCLUDE_DIR}")
endif()


link_directories( ${LLVM_INSTALL_DIR}/lib )

add_subdirectory(src)

if(PLUGIN_HDL) 
  include_directories(plugins/hdl)
  add_subdirectory(plugins)
  set( SRC_LIB libsystemc-clang libhdl )
endif()

if( TESTS_ON )
  add_subdirectory(tests)
  enable_testing()
endif()

if( HW_FLOW_ON )
  add_subdirectory(hardware)
endif()


add_executable(
  ${TARGET}
  ${ROOT_CPP} 
  )

target_link_libraries(${TARGET} ${SRC_LIB}) 

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
message(STATUS "Plugin HDL           : ${PLUGIN_HDL}")
message(STATUS "")
message(STATUS "[ Tests         ]")
message(STATUS "Build tests           : ${TESTS_ON}")
message(STATUS "Build Verilog tests   : ${V_TESTS_ON}")
message(STATUS "")
message(STATUS "[ Environment ]")
message(STATUS "CMAKE_GENERATOR       : ${CMAKE_GENERATOR}")
message(STATUS "CMAKE_BUILD_TYPE      : ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_HOST_NAME       : ${CMAKE_HOST_NAME}")
message(STATUS "Compiler ID           : ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Compiler version      : ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Compiler path         : ${CMAKE_CXX_COMPILER}")
message(STATUS "")
message(STATUS "clang version         : ${CLANG_VERSION}")
message(STATUS "clang path            : ${LLVM_INSTALL_DIR}")
message(STATUS "llvm-config           : ${LLVM_CONFIG_EXE}")
message(STATUS "Use ccache            : ${USE_CCACHE}")
message(STATUS "")
message(STATUS "SystemC path          : ${SYSTEMC_DIR}")
message(STATUS "")
message(STATUS "Install path          : ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_SOURCE_DIR      : ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR      : ${CMAKE_BINARY_DIR}")
message(STATUS "EXTRA_INCLUDE_DIR     : ${EXTRA_INCLUDE_DIR}")
message(STATUS "")
message(STATUS "Python found          : ${PYTHONINTERP_FOUND}")
message(STATUS "Python interpreter    : ${PYTHON_EXECUTABLE}")
message(STATUS "")
message(STATUS "[ Docymentation ]")
message(STATUS "Build documentation   : ${BUILD_DOC}")
if (BUILD_DOC)
message(STATUS "Doxygen found         : ${DOXYGEN_FOUND}")
message(STATUS "Sphinx found          : ${SPHINX_FOUND}")
endif (BUILD_DOC)
message(STATUS "")
message(STATUS "[ Git info.     ]")
message(STATUS "Git repo url          : ${GIT_REPO_URL}")
message(STATUS "Git commit hash       : ${GIT_COMMIT_HASH}")
message(STATUS "")
