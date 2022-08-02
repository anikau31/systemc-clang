# cmake_minimum_required(VERSION 3.3)

# project(synthesis)

set(BD_DIR     bd)
set(RTL_DIR    rtl)
set(IP_DIR     ip)
set(VERIF_DIR  verif)
set(CONSTR_DIR constr)
set(APP_DIR    app)


set(SYNTH_ROOT_DIR ${CMAKE_BINARY_DIR}/hardware)
set(HW_SOURCE_DIR  ${PROJECT_SOURCE_DIR}/hardware)

set(IP_USER_FILES ${SYNTH_ROOT_DIR}/ip_user_files)
set(IP_STATIC     ${SYNTH_ROOT_DIR}/ip_user_files/ip_static)
file(MAKE_DIRECTORY ${IP_USER_FILES})
file(MAKE_DIRECTORY ${IP_STATIC})


include(Targets.cmake)

# global settings
set(COMMON_DIR ${SYNTH_ROOT_DIR}/common.tcl)
if(NOT DEFINED BOARD) # default setup
  set(BOARD "ultra96v2")  # default board
endif()

if(${BOARD} STREQUAL "pynqz1") # pynq z1 board
  set(PART xc7z020clg400-1)
  set(BOARDPART www.digilentinc.com:pynq-z1:part0:1.0)
elseif(${BOARD} STREQUAL "ultra96v2") # ultra96v2 board
  set(PART xczu3eg-sbva484-1-i)
  set(BOARDPART avnet.com:ultra96v2:part0:1.2)
elseif(${BOARD} STREQUAL "ultra96v2_20211") # ultra96v2 board, but for Vivado 2021.1 boardfiles
  set(PART xczu3eg-sbva484-1-i)
  set(BOARDPART avnet.com:ultra96v2:part0:1.2)
elseif(${BOARD} STREQUAL "ultra96v2_20221") # ultra96v2 board, but for Vivado 2022.1 boardfiles
  set(PART xczu3eg-sbva484-1-i)
  set(BOARDPART avnet.com:ultra96v2:part0:1.2)
endif()

configure_file(common.tcl.config common.tcl)
message(STATUS "BOARD: ${BOARD} (${BOARDPART})")
message(STATUS "PART:  ${PART}")

add_subdirectory(${BD_DIR})  # block design
add_subdirectory(${RTL_DIR})
add_subdirectory(${IP_DIR}) 
add_subdirectory(${VERIF_DIR}) # verification
add_subdirectory(${CONSTR_DIR}) # constraints
# add_subdirectory(driver)  # only useful for accelerator
add_subdirectory(${APP_DIR}) # application software/scripts

