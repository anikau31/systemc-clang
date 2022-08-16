function(add_static_synthesis design rtl_files constraint_files synthesis_options bds ips external_dcps top_module sysc_modules)
  set(bd_dcps "")
  set(bd_wrappers ${bds})
  set(ip_dcps "")
  set(ip_wrappers ${ips})
  set(ips_dep ${ips})

  message(STATUS "SYSC MODULES1: ${sysc_modules}")

  # board design wrappers
  set(bd_wrappers_space_sep "")
  foreach(bd ${bd_wrappers})
    # string(APPEND bd_wrappers_space_sep "${CMAKE_CURRENT_BINARY_DIR}/../bd/${bd}/.srcs/sources_1/bd/${bd}/hdl/${bd}_wrapper.v ")
    string(APPEND bd_wrappers_space_sep "${CMAKE_CURRENT_BINARY_DIR}/../bd/${bd}/.gen/sources_1/bd/${bd}/hdl/${bd}_wrapper.v ")
    list(APPEND bd_dcps "bd/${bd}.dcp")
  endforeach()

  # ip stubs
  set(ip_wrappers_space_sep "")
  foreach(ip ${ip_wrappers})
    # string(APPEND ip_wrappers_space_sep "${CMAKE_CURRENT_BINARY_DIR}/../ip/${ip}/.srcs/sources_1/ip/${ip}/${ip}_stub.v ")
    string(APPEND ip_wrappers_space_sep "${CMAKE_CURRENT_BINARY_DIR}/../ip/${ip}/.gen/sources_1/ip/${ip}/${ip}_stub.v ")
    # list(APPEND ip_dcps "ip/${ip}/${ip}.dcp")
    list(APPEND ip_dcps "ip/${ip}/.gen/sources_1/ip/${ip}/${ip}.dcp")
  endforeach()


  set(dcps ${bd_dcps} ${ip_dcps})
  set(dcps ${dcps} ${external_dcps})


  set(dcps_dep ${bd_dcps} ${external_dcps})
  # set(dcps_dep ${dcps})
  list(TRANSFORM rtl_files        PREPEND "${HW_SOURCE_DIR}/rtl/")
  list(TRANSFORM sysc_modules     PREPEND "${SYNTH_ROOT_DIR}/rtl/")
  list(TRANSFORM sysc_modules     APPEND  ".sv")
  list(TRANSFORM constraint_files PREPEND "${HW_SOURCE_DIR}/constr/")
  list(TRANSFORM dcps             PREPEND "${CMAKE_CURRENT_BINARY_DIR}/../")
  list(TRANSFORM dcps_dep         PREPEND "${CMAKE_CURRENT_BINARY_DIR}/../")
  # list(TRANSFORM ips_dep          PREPEND "${SYNTH_ROOT_DIR}/ip/")
  STRING (REPLACE ";" " " rtl_files_space_sep "${rtl_files}")
  STRING (REPLACE ";" " " sysc_modules_space_sep "${sysc_modules}")
  STRING (REPLACE ";" " " constraint_files_space_sep "${constraint_files}")
  STRING (REPLACE ";" " " dcp_files_space_sep "${dcps}")

  message(STATUS "Design (top module name): ${design}")
  message(STATUS "SYSC MODULES: ${sysc_modules}")

  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${design})
  set(working_dir ${CMAKE_CURRENT_BINARY_DIR}/${design}/)
  set(synthesis_tcl ${working_dir}/synthesis.tcl)
  if (NOT "${sysc_modules}" STREQUAL "")
    set(add_sysc_command "add_files { ${sysc_modules_space_sep} }")
  endif()
  if (NOT "${bds}" STREQUAL "")
    set(add_bd_command "add_files { ${bd_wrappers_space_sep} }")
  endif()
  if (NOT "${ips}" STREQUAL "")
    set(add_ip_command "add_files { ${ip_wrappers_space_sep} }")
  endif()
  if(NOT "${xdc}" STREQUAL "")
    set(add_xdc_command "add_files { ${constraint_files_space_sep} }")
  endif()
  if(NOT "${dcps}" STREQUAL "")
    set(add_dcp_command "add_files { ${dcp_files_space_sep} }")
  endif()

  configure_file(${HW_SOURCE_DIR}/rtl/synthesis.tcl ${synthesis_tcl})

  # message(STATUS "${dcps_dep}")

  # list(TRANSFORM bds              PREPEND "${HW_SOURCE_DIR}/bd/")
  list(TRANSFORM bds              APPEND  ".bd")
  add_custom_command(OUTPUT ${design}/${design}_synth.dcp
    COMMAND vivado -mode batch -source  -source ${COMMON_DIR} -source ${synthesis_tcl}
    DEPENDS ${dcps_dep} ${rtl_files} ${bds} ${constraint_files} ${ips_dep} ${sysc_modules}
    WORKING_DIRECTORY ${working_dir})


  add_custom_target(${design}_synth DEPENDS ${design}/${design}_synth.dcp)


endfunction()


# the reconf_partition should have the full path name
function(add_reconf_module) # design reconf_partition reconf_module rtl_files constraint_files synthesis_options bds ips)
  set(oneValArgs DESIGN RECONF_PART RECONF_INST SYNTHESIS_OPTS)
  set(multiValArgs RTL SYSC_MODULE CONSTR BDS IPS DCPS)
  cmake_parse_arguments(RM "" "${oneValArgs}" "${multiValArgs}" ${ARGN})

  set(RM_SYNTHESIS_OPTS "${RM_SYNTHESIS_OPTS} -mode out_of_context")
  message(STATUS "Reconfigurable module (name:modname:partition): ${RM_DESIGN}:${RM_RECONF_PART}:${RM_RECONF_INST}")
  message(STATUS "RTL files: ${RM_RTL}")
  message(STATUS "SystemC modules: ${RM_SYSC_MODULE}")
  message(STATUS "Constraints: ${RM_CONSTR}")
  message(STATUS "Synthesis options: ${RM_SYNTHESIS_OPTS}")
  message(STATUS "Deps (not supported for now): ${RM_BDS} ${RM_IPS} ${RM_DCPS}")

  add_static_synthesis(RM_${RM_DESIGN} 
    "${RM_RTL}" 
    "${RM_CONSTR}" "${RM_SYNTHESIS_OPTS}" "${RM_BDS}" "${RM_IPS}" "${RM_DCPS}" ${RM_RECONF_PART} "${RM_SYSC_MODULE}")
  message(STATUS "")
endfunction()

function(add_static_implementation)
  # Argument parsing
  set(oneValArgs DESIGN SYNTH_DCP)
  set(multiValArgs RECONF_PART RECONF_INST RECONF_MOD CONSTR)
  cmake_parse_arguments(DS "" "${oneValArgs}" "${multiValArgs}" ${ARGN})

  message(STATUS "Implementation: DS_${DS_DESIGN}")

  # Set up constraints
  set(design DS_${DS_DESIGN})
  list(TRANSFORM DS_CONSTR PREPEND "${HW_SOURCE_DIR}/constr/")
  if(NOT "${DS_CONSTR}" STREQUAL "")
    STRING(JOIN " " constr_files ${DS_CONSTR})
    message(STATUS "${DS_CONSTR}")
    set(add_xdc_command "read_xdc { ${constr_files} }")
  endif()

  # Set up HD.RECONFIGURABLE properties for each reconf partition
  # Also, we need to create black boxes for those reconf partition so that we can implement later
  set(reconf_command_list "")
  set(blackboxing_command_list "")
  list(LENGTH DS_RECONF_PART len1)
  math(EXPR DS_RECONF_INDICES "${len1} - 1")
  foreach(idx RANGE ${DS_RECONF_INDICES})
    list(GET DS_RECONF_PART ${idx} part)
    list(GET DS_RECONF_INST ${idx} inst)
    list(GET DS_RECONF_MOD ${idx} mod)

    message(STATUS "RPartition ${part}:${inst} instantiated with ${mod}")
    # list(APPEND reconf_command_list "set_property HD.RECONFIGURABLE true [get_cells ${inst}]")

    list(APPEND blackboxing_command_list "update_design -cell ${inst} -black_box")
  endforeach()
  string(JOIN "\n" reconf_command ${reconf_command_list})
  string(JOIN "\n" blackboxing_command ${blackboxing_command_list})

  set(lock_design_if_reconf "")
  set(lock_dcp "")
  if(NOT "${reconf_command_list}" STREQUAL "")
    set(lock_design_if_reconf "lock_design -level routing\nwrite_checkpoint -force ${design}_locked_routed.dcp")
    set(lock_dcp "${design}/${design}_locked_routed.dcp")
  endif()



  set(working_dir ${CMAKE_CURRENT_BINARY_DIR}/${design}/)
  set(implementation_tcl ${working_dir}/implementation.tcl)
  set(post_check_tcl ${working_dir}/post_check.tcl)
  set(bitstream_tcl ${working_dir}/bitstream.tcl)
  set(utilization_tcl ${working_dir}/utilization.tcl)

  configure_file(${HW_SOURCE_DIR}/rtl/implementation.tcl ${implementation_tcl})
  configure_file(${HW_SOURCE_DIR}/rtl/post_check.tcl ${post_check_tcl})
  configure_file(${HW_SOURCE_DIR}/rtl/bitstream.tcl ${bitstream_tcl})
  configure_file(${HW_SOURCE_DIR}/rtl/utilization.tcl ${utilization_tcl})
  
  message(STATUS "DS_SYNTH_DCP: ${DS_SYNTH_DCP}")
  add_custom_command(OUTPUT ${design}/${design}_impl.dcp ${lock_dcp}
    COMMAND vivado -mode batch -source ${COMMON_DIR} -source ${implementation_tcl}
    DEPENDS ${DS_SYNTH_DCP} ${DS_CONSTR}
    WORKING_DIRECTORY ${working_dir})
  add_custom_target(${design}_post_check
    COMMAND vivado -mode batch -source ${COMMON_DIR} -source ${post_check_tcl}
    DEPENDS ${design}/${design}_impl.dcp
    WORKING_DIRECTORY ${working_dir})
  add_custom_command(OUTPUT ${design}/${design}.bit
    COMMAND vivado -mode batch -source ${COMMON_DIR} -source ${bitstream_tcl}
    DEPENDS ${design}_post_check ${design}/${design}_impl.dcp
    WORKING_DIRECTORY ${working_dir})
  add_custom_command(OUTPUT ${design}/${design}_util.txt
    COMMAND vivado -mode batch -source ${COMMON_DIR} -source ${utilization_tcl}
    DEPENDS ${design}/${design}_impl.dcp ${HW_SOURCE_DIR}/rtl/utilization.tcl 
    WORKING_DIRECTORY ${working_dir})
  add_custom_target(${design}_impl DEPENDS ${design}/${design}_impl.dcp)
  add_custom_target(${design}_bitstream DEPENDS ${design}/${design}.bit)
  add_custom_target(${design}_util DEPENDS ${design}/${design}_util.txt)

endfunction()

function(add_static_config) # design reconf_partition reconf_module rtl_files constraint_files synthesis_options bds ips)
  set(oneValArgs DESIGN SYNTHESIS_OPTS TOP_MOD)
  set(multiValArgs RTL RECONF_PART RECONF_INST RECONF_MOD CONSTR BDS IPS)
  cmake_parse_arguments(DS "" "${oneValArgs}" "${multiValArgs}" ${ARGN})

  message(STATUS "Static design: ${DS_DESIGN}")
  message(STATUS "Static top module: ${DS_TOP_MOD}")
  message(STATUS "RTL files: ${DS_RTL}")
  message(STATUS "Constraints: ${DS_CONSTR}")
  message(STATUS "Synthesis options: ${DS_SYNTHESIS_OPTS}")
  list(LENGTH DS_RECONF_PART len1)
  math(EXPR DS_RECONF_INDICES "${len1} - 1")
  foreach(idx RANGE ${DS_RECONF_INDICES})
    list(GET DS_RECONF_PART ${idx} part)
    list(GET DS_RECONF_INST ${idx} inst)
    list(GET DS_RECONF_MOD ${idx} mod)
    message(STATUS "RPartition ${part}:${inst} instantiated with ${mod}")
    # message(STATUS "  DCPs: RM_${mod}/RM_${mod}_synth.dcp")
    list(APPEND DS_DCPS rtl/RM_${mod}/RM_${mod}_synth.dcp)
  endforeach()
  message(STATUS "Deps (not supported for now): ${DS_BDS} ${DS_IPS} ${DS_DCPS}")

  add_static_synthesis(DS_${DS_DESIGN} 
    "${DS_RTL}" 
    "${DS_CONSTR}" "${DS_SYNTHESIS_OPTS}" "${DS_BDS}" "${DS_IPS}" "${DS_DCPS}" ${DS_TOP_MOD} "")
  message(STATUS ">> Adding implementation for the configuration...")
  add_static_implementation(
    DESIGN      ${DS_DESIGN} 
    SYNTH_DCP   DS_${DS_DESIGN}/DS_${DS_DESIGN}_synth.dcp
    RECONF_PART ${DS_RECONF_PART}
    RECONF_INST ${DS_RECONF_INST}
    RECONF_MOD  ${DS_RECONF_MOD}
    CONSTR      ${DS_CONSTR}     # perhaps floor planning
  )
  message(STATUS "")
endfunction()

function(add_config_from_static)
  set(oneValArgs DESIGN STATIC_DESIGN)
  set(multiValArgs RECONF_PART RECONF_INST RECONF_MOD CONSTR)
  cmake_parse_arguments(DS "" "${oneValArgs}" "${multiValArgs}" ${ARGN})

  message(STATUS "Reconfig design: ${DS_DESIGN}")

  set(locked_dcp ${CMAKE_CURRENT_BINARY_DIR}/DS_${DS_STATIC_DESIGN}/DS_${DS_STATIC_DESIGN}_locked_routed.dcp)
  set(static_dcp ${CMAKE_CURRENT_BINARY_DIR}/DS_${DS_STATIC_DESIGN}/DS_${DS_STATIC_DESIGN}_impl.dcp)
  set(design DS_${DS_DESIGN})
  list(TRANSFORM DS_CONSTR PREPEND "${HW_SOURCE_DIR}/constr/")
  if(NOT "${DS_CONSTR}" STREQUAL "")
    STRING(JOIN " " constr_files ${DS_CONSTR})
    message(STATUS "${DS_CONSTR}")
    set(add_xdc_command "read_xdc { ${constr_files} }")
  endif()

  # Set up HD.RECONFIGURABLE properties for each reconf partition
  # Also, we need to create black boxes for those reconf partition so that we can implement later
  set(reconf_command_list "")
  set(reconf_mod_list "")
  # set(blackboxing_command_list "")
  list(LENGTH DS_RECONF_PART len1)
  math(EXPR DS_RECONF_INDICES "${len1} - 1")
  foreach(idx RANGE ${DS_RECONF_INDICES})
    list(GET DS_RECONF_PART ${idx} part)
    list(GET DS_RECONF_INST ${idx} inst)
    list(GET DS_RECONF_MOD ${idx} mod)

    message(STATUS "RPartition ${part}:${inst} instantiated with ${mod}")
    # list(APPEND reconf_command_list "set_property HD.RECONFIGURABLE true [get_cells ${inst}]")
    # list(APPEND reconf_command_list "set_property HD.RECONFIGURABLE true [get_cells ${inst}]")
    list(APPEND reconf_command_list "read_checkpoint -cell ${inst} ${CMAKE_CURRENT_BINARY_DIR}/RM_${mod}/RM_${mod}_synth.dcp")
    list(APPEND reconf_mod_list "${CMAKE_CURRENT_BINARY_DIR}/RM_${mod}/RM_${mod}_synth.dcp")
  endforeach()
  string(JOIN "\n" add_reconf_command ${reconf_command_list})

  set(working_dir ${CMAKE_CURRENT_BINARY_DIR}/${design}/)
  set(implementation_tcl ${working_dir}/implementation.tcl)
  set(post_check_tcl ${working_dir}/post_check.tcl)
  set(bitstream_tcl ${working_dir}/bitstream.tcl)
  set(utilization_tcl ${working_dir}/utilization.tcl)

  configure_file(${HW_SOURCE_DIR}/rtl/implementation_conf.tcl ${implementation_tcl})
  configure_file(${HW_SOURCE_DIR}/rtl/post_check.tcl ${post_check_tcl})
  configure_file(${HW_SOURCE_DIR}/rtl/bitstream.tcl ${bitstream_tcl})
  configure_file(${HW_SOURCE_DIR}/rtl/utilization.tcl ${utilization_tcl})

  
  message(STATUS "DS_SYNTH_DCP: ${DS_SYNTH_DCP}")
  add_custom_command(OUTPUT ${design}/${design}_impl.dcp
    COMMAND vivado -mode batch -source ${COMMON_DIR} -source ${implementation_tcl}
    DEPENDS ${DS_SYNTH_DCP} ${DS_CONSTR} ${reconf_mod_list} ${static_dcp} ${locked_dcp}
    WORKING_DIRECTORY ${working_dir})
  add_custom_target(${design}_post_check
    COMMAND vivado -mode batch -source ${COMMON_DIR} -source ${post_check_tcl}
    DEPENDS ${design}/${design}_impl.dcp
    WORKING_DIRECTORY ${working_dir})
  add_custom_command(OUTPUT ${design}/${design}.bit
    COMMAND vivado -mode batch -source ${COMMON_DIR} -source ${bitstream_tcl}
    DEPENDS ${design}_post_check ${design}/${design}_impl.dcp
    WORKING_DIRECTORY ${working_dir})
  add_custom_command(OUTPUT ${design}/${design}_util.txt
    COMMAND vivado -mode batch -source ${COMMON_DIR} -source ${utilization_tcl}
    DEPENDS ${design}/${design}_impl.dcp ${HW_SOURCE_DIR}/rtl/utilization.tcl 
    WORKING_DIRECTORY ${working_dir})
  add_custom_target(${design}_impl DEPENDS ${design}/${design}_impl.dcp)
  add_custom_target(${design}_bitstream DEPENDS ${design}/${design}.bit)
  add_custom_target(${design}_util DEPENDS ${design}/${design}_util.txt)

endfunction()

function(add_systemc_module)
  # SystemC C++ source
  set(oneValArgs DESIGN DESIGN_SOURCE)
  # EXTRA_COMPILER_OPTIONS are the options passed to the systemc-clang binary
  # EXTRA_HCODE_OPTIONS are the options passed to the hcode translation script
  set(multiValArgs EXTRA_COMPILER_OPTIONS EXTRA_HCODE_OPTIONS) 
  cmake_parse_arguments(SYSC "" "${oneValArgs}" "${multiValArgs}" ${ARGN})

  message(STATUS "SystemC design: ${SYSC_DESIGN_SOURCE}")

  set(SYSTEMC_CLANG_FLAGS "-x" "c++" "-w" "-c" "-D__STDC_CONSTANT_MACROS" "-D__STDC_LIMIT_MACROS -DRVD" "-I/opt/clang-13.0.0/lib/clang/13.0.0/include/" "-I/usr/include" "-I/opt/systemc-2.3.3/include")
  get_filename_component(DESIGN_SOURCE_FILENAME ${SYSC_DESIGN_SOURCE} NAME_WLE)
  set(SYSC_DESIGN_TARGET "${SYSC_DESIGN}")
  message(STATUS "Design source ${DESIGN_SOURCE_FILENAME}")
  
  add_custom_command(
    OUTPUT ${SYNTH_ROOT_DIR}/rtl/${SYSC_DESIGN_TARGET}.sv
    COMMAND "${PYTHON_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/plugins/hdl/systemc-clang.py"
    ${SYSC_DESIGN_SOURCE}
    "--" ${SYSTEMC_CLANG_FLAGS} ${EXTRA_COMPILER_OPTIONS} "---" "-o" "${SYNTH_ROOT_DIR}/rtl/${SYSC_DESIGN_TARGET}.sv"
  )
  add_custom_target(${SYSC_DESIGN}
    DEPENDS ${SYNTH_ROOT_DIR}/rtl/${SYSC_DESIGN_TARGET}.sv
  )

endfunction()

