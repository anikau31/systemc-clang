set(IP_SCRIPT generate_bd_dcp.tcl)
set(IP_DIR ${HW_SOURCE_DIR}/ip)
set(IP_LIST )


function(add_ip_dcp input)
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${input})

  set(working_dir ${CMAKE_CURRENT_BINARY_DIR}/${input}/)
  set(gen_sim_tcl ${working_dir}/gen_sim.tcl)
  configure_file(${HW_SOURCE_DIR}/ip/gen_sim.tcl ${gen_sim_tcl})
  add_custom_command(
    OUTPUT ${input}/.srcs/sources_1/ip/${input}/${input}.dcp ${input}/${input}.dcp
    # remove hidden files created by Vivado, which will not be overwritten on a new build
    COMMAND ${CMAKE_COMMAND} -E echo "removing ${CMAKE_CURRENT_BINARY_DIR}/${input}/.*"
    COMMAND ${CMAKE_COMMAND} -E remove_directory -rf ${CMAKE_CURRENT_BINARY_DIR}/${input}/.hbs
    COMMAND ${CMAKE_COMMAND} -E remove_directory -rf ${CMAKE_CURRENT_BINARY_DIR}/${input}/.srcs
    COMMAND ${CMAKE_COMMAND} -E remove_directory -rf ${CMAKE_CURRENT_BINARY_DIR}/${input}/.Xil
    COMMAND vivado -mode batch -nojournal -nolog -source ${COMMON_DIR} -source ${IP_DIR}/${input}.tcl -source ${gen_sim_tcl}
    COMMAND ${CMAKE_COMMAND} -E copy .srcs/sources_1/ip/${input}/${input}.dcp ${input}.dcp
    DEPENDS ${IP_DIR}/${input}.tcl
    COMMENT "Generating board design dcp for ${input}"
    SOURCES ${IP_DIR}/${input}.tcl
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${input}
    VERBATIM
  )
  add_custom_target(${input} DEPENDS ${input}/.srcs/sources_1/ip/${input}/${input}.dcp ${input}/${input}.dcp)

endfunction()

add_ip_dcp(axi_bram_blank)
add_ip_dcp(axis_dwidth_256_32)
add_ip_dcp(axis_dwidth_32_136)
add_ip_dcp(axis_dwidth_32_256)
add_ip_dcp(axis_dwidth_32_64)
add_ip_dcp(axis_dwidth_64_32)
add_ip_dcp(axis_interconnect_1_2)

