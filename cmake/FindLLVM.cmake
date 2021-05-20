
# Find llvm-config to setup the remaining paths.
# switch to the variable named VARNAME.
#
# Example usage: llvm_config(LLVM_CXXFLAGS "--cxxflags")
function(llvm_config VARNAME switch comp)
  set(CONFIG_COMMAND ${LLVM_CONFIG_EXE} ${switch} ${comp})

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

  # Clang 11.0.0 generates -lxml2, which causes issues on OSX.
  # For now, just remove it as it is produced from --system-libs.
  #if (APPLE ) 
  string(REGEX REPLACE " -lxml2" "" CONFIG_OUTPUT "${CONFIG_OUTPUT}")
  string(REGEX REPLACE " -llibxml2.tbd" "" CONFIG_OUTPUT "${CONFIG_OUTPUT}") 
  #endif()

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
    string(REPLACE "-std=c++14" "-std=c++17" CONFIG_OUTPUT "${CONFIG_OUTPUT}")
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
  message(FATAL_ERROR "
	FATAL: llvm-config not found.
				 Please review LLVM/Clang installation. -- ${LLVM_CONFIG_EXE} ")
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


