set(BD_SCRIPT generate_bd_dcp.tcl)
set(BD_DIR ${HW_SOURCE_DIR}/bd)
set(BD_LIST )

function(add_bd_dcp input)
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${input})
  add_custom_command(OUTPUT ${input}.dcp
    COMMAND vivado -mode batch -nojournal -nolog -source ${COMMON_DIR} -source ${BD_DIR}/generate_bd_dcp.tcl -tclargs ${input} ${BD_DIR}/${input}_${BOARD}.tcl && cp ${input}.dcp ..
    DEPENDS ${BD_DIR}/generate_bd_dcp.tcl  ${BD_DIR}/${input}_${BOARD}.tcl
    COMMENT "Generating board design dcp for ${input}"
    SOURCES ${BD_DIR}/${input}_${BOARD}.tcl
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${input}
    VERBATIM
  )
  add_custom_target(${input}.bd DEPENDS ${input}.dcp)
endfunction()


add_bd_dcp(bd)

