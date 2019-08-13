cmake_minimum_required(VERSION 2.6)
project(systemc-clang)
# Use C++ standard version to c++17
set(CMAKE_CXX_STANDARD 14)
# Always keep it on
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#  Make verbose on?
#set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

# Required packages
# TODO: Add the code to find library and/or the specific in-source file.
# find_package(nlohmann_json 3.2.0 REQUIRED)


# Include the these flags whenever you compile.
add_definitions (-D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS)

#add_definitions (-D_GNU_SOURCE -DHAVE_CLANG_CONFIG_H)

option (USE_SAUTO
  "Use suspension-automata library." OFF
  )

option (USE_SILENT_BUILD
  "Silent as many warnings as possible." OFF
  )

set (ROOT_CPP
  driver-tooling.cpp
  )

set (LLVM_BUILD_DIR
  $ENV{LLVM_BUILD_DIR}
  )

set (CMAKE_CXX_FLAGS
   "$ENV{LLVM_CXX_FLAGS} $ENV{CMAKE_CXX_FLAGS} "
  )

if (USE_SAUTO)
  set (CMAKE_CXX_FLAGS
    "-DUSE_SAUTO ${CMAKE_CXX_FLAGS} "
    )
endif(USE_SAUTO)

if (USE_SILENT_BUILD)
  set (CMAKE_CXX_FLAGS
    "-DUSE_SILENT_BUILD=on ${CMAKE_CXX_FLAGS} "
    )
endif(USE_SILENT_BUILD)


set (LLVM_LD_FLAGS
  "$ENV{LLVM_LD_FLAGS} -lpthread"
  )

set (LLVM_LIBS
  $ENV{LLVM_LIBS}
  )

#set (LLVM_LD_LIBS LLVMCppBackendCodeGen LLVMCppBackendInfo LLVMTarget LLVMCore LLVMMC LLVMObject LLVMSupport)

#LLVMJIT LLVMX86CodeGen LLVMX86AsmParser LLVMX86Disassembler LLVMExecutionEngine LLVMAsmPrinter LLVMSelectionDAG LLVMX86AsmPrinter LLVMX86Info LLVMMCParser LLVMCodeGen LLVMX86Utils LLVMScalarOpts LLVMInstCombine LLVMTransformUtils LLVMipa LLVMAnalysis LLVMTarget LLVMCore LLVMMC LLVMSupport

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
  $ENV{LLVM_LD_FLAGS}
  )

set (TARGET
  "systemc-clang"
  )


include_directories(
  "${LLVM_BUILD_DIR}/include"
  src
  if (USE_SAUTO)
    sauto
  endif (USE_SAUTO)

  )


link_directories(
  ${LLVM_BUILD_DIR}/lib
  )

add_subdirectory(src)
add_subdirectory(tests)

if (USE_SAUTO)
  add_subdirectory(sauto)
endif (USE_SAUTO)


set( SRC_LIB libsystemc-clang )
if (USE_SAUTO)
  set( SAUTO_LIB libsauto )
endif (USE_SAUTO)
add_executable(
  ${TARGET}
  ${ROOT_CPP} )

target_link_libraries(${TARGET} ${SRC_LIB}) #nlohmann_json::nlohmann_json)
target_link_libraries(${TARGET} ${SAUTO_LIB}) # nlohmann_json::nlohmann_json)

foreach (clang_lib ${CLANG_LIBS})
  target_link_libraries( ${TARGET} ${clang_lib} )
endforeach()

foreach (ld_flag ${LLVM_LD_FLAGS})
  target_link_libraries( ${TARGET} ${ld_flag} )
endforeach()

foreach (llvm_lib ${LLVM_LIBS})
  target_link_libraries( ${TARGET} ${llvm_lib} )
endforeach()
