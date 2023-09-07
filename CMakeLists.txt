cmake_minimum_required(VERSION 3.13.4)
project(sccl)

# Sources
# 1. https://github.com/ttroy50/cmake-examples/tree/master
# 2. https://github.com/banach-space/CLANG-TUTOR

#===============================================================================
# 0. GET CLANG INSTALLATION DIR
#===============================================================================
# In clang-tutor, `SCCL_Clang_INSTALL_DIR` is the key CMake variable - it points
# to a Clang installation directory. For the sake of completeness,
# <PackageName>_DIR (i.e. `Clang_DIR`) and <PackageName>_ROOT (i.e.
# `Clang_ROOT`) are also supported. Visit CMake documentation for more details:
#   https://cmake.org/cmake/help/latest/command/find_package.html
# Use only _one_ of these variables.

#===============================================================================
# Set all  the necessary flags.
#===============================================================================

# Use c++17
set(CMAKE_CXX_STANDARD 17)

# Always keep it on
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#  Make verbose on?
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

# SystemC-Clang versions
set(SCC_MAJOR_VERSION 2)
set(SCC_MINOR_VERSION ${SCC_MAJOR_VERSION}.2)

# ===========================================
# Options
# ===========================================
option(BUILD_DOC "Build with documentation targets" OFF)

# HDL plugin
# Thank you LLNL and Maya!
option( HDL "Build with HDL plugin" OFF )
option( ENABLE_TESTS "Build tests" OFF )
option( ENABLE_VERILOG_TESTS "Build Verilog tests" OFF )
option( HW_FLOW "Build hardware flow" OFF )
option( USE_CCACHE "Build using ccache" OFF )

set( EXTRA_INCLUDE_DIR $ENV{EXTRA_INCLUDE_DIR} )
# set( SCCL_LIBS sccl "clangTooling")
#
# if(HDL)
#     set(SCCL_LIBS ${SCCL_LIBS} hdl)
# endif()
#
if(NOT "${EXTRA_INCLUDE_DIR}" STREQUAL "")
  include_directories( "${EXTRA_INCLUDE_DIR}")
endif()


if (ENABLE_VERILOG_TESTS)
  # find_package(PythonInterp 3 REQUIRED)
  find_package(Python3 COMPONENTS Interpreter)
endif()

if (HW_FLOW)
  find_package(Python3 COMPONENTS Interpreter)
  # find_package(PythonInterp 3 REQUIRED)
  if (NOT HDL)
      message(WARNING "WARNING: HDL enabled. HDL must be turned on in order to run hardware flow")
  endif() 
  set( HDL ON )
endif()

if (USE_CCACHE)
find_program(CCACHE_PROGRAM ccache)
  if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    set(CMAKE_C_COMPILER_LAUNCHER   ${CCACHE_PROGRAM})
  endif()
endif()

# ===========================================
# Documentation packages
# ===========================================

if (BUILD_DOC)
##################################################
# Sphinx
##################################################
find_program(SPHINX_EXECUTABLE
             NAMES sphinx-build sphinx-build2
             DOC "Path to sphinx-build executable")

# Handle REQUIRED and QUIET arguments
# this will also set SPHINX_FOUND to true if SPHINX_EXECUTABLE exists
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Sphinx
                                  "Failed to locate sphinx-build executable"
                                  SPHINX_EXECUTABLE)

# Provide options for controlling different types of output
option(SPHINX_OUTPUT_HTML "Output standalone HTML files" ON)
option(SPHINX_OUTPUT_MAN "Output man pages" ON)

option(SPHINX_WARNINGS_AS_ERRORS "When building documentation treat warnings as errors" ON)

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

# ===========================================
# Testing
# ===========================================
include(CTest)
set(TESTING_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals)

# ===========================================
# Build paths
# ===========================================

set( LLVM_INSTALL_DIR $ENV{LLVM_INSTALL_DIR} )
set( SYSTEMC_DIR $ENV{SYSTEMC} )

if ("${LLVM_INSTALL_DIR}" STREQUAL "")
    message(FATAL_ERROR "
    FATAL: LLVM_INSTALL_DIR not set.
    Specify the path to Clang using LLVM_INSTALL_DIR. ${HAD_ERROR}")
endif()

if (NOT SYSTEMC_DIR) #("${SYSTEMC_DIR}" STREQUAL "")
    message(FATAL_ERROR "
    FATAL: SYSTEMC_DIR not set.
           Specify the path to SystemC in SYSTEMC_DIR. ${HAD_ERROR}")
endif()

if (DEFINED LLVM_INSTALL_DIR)
  set(Clang_ROOT ${LLVM_INSTALL_DIR})
  set(SCCL_CLANG_PACKAGE_DIR "${Clang_ROOT}")
endif()
mark_as_advanced(SCCL_CLANG_PACKAGE_DIR)


# if(DEFINED Clang_ROOT)
#   set(SCCL_CLANG_PACKAGE_DIR "${Clang_ROOT}")
# elseif(DEFINED Clang_DIR)
#   set(SCCL_CLANG_PACKAGE_DIR "${Clang_DIR}")
# endif()
# mark_as_advanced(SCCL_CLANG_PACKAGE_DIR)

# Set this to a valid Clang installation directory. This is most likely where
# LLVM is installed on your system.
set(SCCL_Clang_INSTALL_DIR "${SCCL_CLANG_PACKAGE_DIR}" CACHE PATH
  "Clang installation directory")

message(STATUS "SCCL_Clang_INSTALL_DIR ${SCCL_CLANG_PACKAGE_DIR}" )
message(STATUS "Clang install dir ${SCCL_Clang_INSTALL_DIR}")

# ===========================================
# Disallow in-source builds
# ===========================================
# Source: https://github.com/opencv/opencv/pull/8582/files<Paste>
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}") 
  message(FATAL_ERROR "
  FATAL: In-source builds are prohibited.
         Create a separate directory for build files.")
endif() 


#===============================================================================
# 1. VERIFY CLANG INSTALLATION DIR
#===============================================================================
set( SCCL_LLVM_INCLUDE_DIR "${SCCL_Clang_INSTALL_DIR}/include/llvm")
# set(CT_LLVM_INCLUDE_DIR "${SCCL_Clang_INSTALL_DIR}/include/llvm")
if(NOT EXISTS "${SCCL_LLVM_INCLUDE_DIR}")
message(FATAL_ERROR
  " SCCL_Clang_INSTALL_DIR (${SCCL_LLVM_INCLUDE_DIR}) is invalid.")
endif()

set(SCCL_LLVM_CMAKE_FILE
  "${LLVM_INSTALL_DIR}/lib/cmake/clang/ClangConfig.cmake")
if(NOT EXISTS "${SCCL_LLVM_CMAKE_FILE}")
message(FATAL_ERROR
  " SCCL_LLVM_CMAKE_FILE (${SCCL_LLVM_CMAKE_FILE}) is invalid.")
endif()

#===============================================================================
# 2. LOAD CLANG CONFIGURATION
#    Extracted from:
#    http://llvm.org/docs/CMake.html#embedding-llvm-in-your-project
#===============================================================================
list(APPEND CMAKE_PREFIX_PATH "${LLVM_INSTALL_DIR}/lib/cmake/clang/")

find_package(Clang REQUIRED CONFIG)

# Sanity check. As Clang does not expose e.g. `CLANG_VERSION_MAJOR` through
# AddClang.cmake, we have to use LLVM_VERSION_MAJOR instead.
# TODO: Revisit when next version is released.
# FIXME: This really needs to be done automatically
set(CLANG_VERSION 15.0.6)
message(STATUS "LLVM_VERSION_MAJOR ${LLVM_VERSION_MAJOR}")
if(NOT "15" VERSION_EQUAL "${LLVM_VERSION_MAJOR}")
  message(FATAL_ERROR "Found LLVM ${LLVM_VERSION_MAJOR}, but need LLVM 15")
endif()
#
# message(STATUS "Found Clang ${LLVM_PACKAGE_VERSION}")
# message(STATUS "Using ClangConfig.cmake in: ${SCCL_Clang_INSTALL_DIR}")
#
# message("CLANG STATUS:
#   Includes (clang)    ${CLANG_INCLUDE_DIRS}
#   Includes (llvm)     ${LLVM_INCLUDE_DIRS}"
# )

# Set the LLVM and Clang header and library paths
include_directories(SYSTEM 
  "${LLVM_INCLUDE_DIRS}"
  "${CLANG_INCLUDE_DIRS}"
  "${TESTING_INCLUDE_DIR}/doctest"
  "src/"
  "src/matchers"
  "src/model"
  "src/cfg"
  "src/utils"
  "src/SCuitable"
  )

#===============================================================================
# 3. BUILD CONFIGURATION
#===============================================================================
# Use the same C++ standard as LLVM does
set(CMAKE_CXX_STANDARD 17 CACHE STRING "")

# Build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE
      STRING "Build type (default Debug):" FORCE)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall\
    -fdiagnostics-color=always")

# LLVM/Clang is normally built without RTTI. Be consistent with that.
if(NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

# -fvisibility-inlines-hidden is set when building LLVM and on Darwin warnings
# are triggered if llvm-tutor is built without this flag (though otherwise it
# builds fine). For consistency, add it here too.
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-fvisibility-inlines-hidden"
  SUPPORTS_FVISIBILITY_INLINES_HIDDEN_FLAG)
if(${SUPPORTS_FVISIBILITY_INLINES_HIDDEN_FLAG} EQUAL "1")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
endif()

# Set the build directories
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/")
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

#===============================================================================
# 4. ADD SUB-TARGETS
# Doing this at the end so that all definitions and link/include paths are
# available for the sub-projects.
#===============================================================================
add_subdirectory(src)
add_subdirectory(tests)

set(SCCL systemc-clang)

if(HDL)
  set(sccl_SOURCE driver-hdl.cpp )
elseif()
  set(sccl_SOURCE driver-tooling.cpp )
endif()


# CONFIGURE THE TOOLS
# ===================
foreach( tool ${SCCL} )
  # Create a library corresponding to 'plugin'
  add_executable(
    ${tool}
    ${CMAKE_SOURCE_DIR}/${sccl_SOURCE} # ${${tool}_SOURCES}
  )

  # Configure include directories for 'tool'
  target_include_directories(
    ${tool}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/plugins/hdl/
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/model/
    ${CMAKE_CURRENT_SOURCE_DIR}/cfg/
    ${CMAKE_CURRENT_SOURCE_DIR}/SCuitable/
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/matchers/
  )

  # Link in the required libraries 
  target_link_libraries(${tool}
    sccl
    hdl
    "clangTooling"
  )
endforeach()
# Print the summary of the build. 
message(STATUS "")
message(STATUS "[ Build summary ]")
message(STATUS "systemc-clang         : ${SCC_MINOR_VERSION}")
message(STATUS "")
message(STATUS "[ Plugins       ]")
message(STATUS "Plugin HDL            : ${HDL}")
message(STATUS "")
message(STATUS "[ Tests         ]")
message(STATUS "Build tests           : ${ENABLE_TESTS}")
message(STATUS "Build Verilog tests   : ${ENABLE_VERILOG_TESTS}")
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
# message(STATUS "llvm-config           : ${LLVM_CONFIG_EXE}")
message(STATUS "Use ccache            : ${USE_CCACHE}")
message(STATUS "")
message(STATUS "SystemC path          : ${SYSTEMC_DIR}")
message(STATUS "")
message(STATUS "Install path          : ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_SOURCE_DIR      : ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR      : ${CMAKE_BINARY_DIR}")
message(STATUS "EXTRA_INCLUDE_DIR     : ${EXTRA_INCLUDE_DIR}")
message(STATUS "")
message(STATUS "Python interpreter    : ${Python3_Interpreter_FOUND}")
message(STATUS "Python executable     : ${Python3_EXECUTABLE}")
message(STATUS "")
message(STATUS "[ Documentation ]")
message(STATUS "Build documentation   : ${BUILD_DOC}")
if (BUILD_DOC)
message(STATUS "Doxygen found         : ${DOXYGEN_FOUND}")
message(STATUS "Sphinx found          : ${SPHINX_FOUND}")
endif (BUILD_DOC)
message(STATUS "")
message(STATUS "")
