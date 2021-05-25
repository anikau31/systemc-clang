function(add_behavioral_sim test design bd_deps file_deps ips) # test is the test name in tests/${test}.sv
  # design is the file list name  ${design}.f
  if(EXISTS ${HW_SOURCE_DIR}/verif/${design}.f)
  else()
    message(FATAL_ERROR "In ${test}, verif/${design}.f should be present, but not found.")
  endif()
  if(EXISTS ${HW_SOURCE_DIR}/verif/tests/${test}.sv)
  else()
    message(FATAL_ERROR "In ${test}, verif/tests/${test}.sv should be present, but not found.")
  endif()
  message(STATUS ${file_deps})

  configure_file(${design}.f ${design}.f)
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${test}_behav)
  file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  set(add_ip_sim_command "")
  foreach(ip IN LISTS ips)
    # string(APPEND add_ip_sim_command "add_files -fileset sim_1 ${CMAKE_CURRENT_BINARY_DIR}/../ip/${ip}/.srcs/sources_1/ip/${ip}/sim/${ip}.v\n")
    string(APPEND add_ip_sim_command "add_files -fileset sim_1 ${CMAKE_CURRENT_BINARY_DIR}/../ip/${ip}/.srcs/sources_1/ip/${ip}/${ip}.xci\ngenerate_target simulation [get_files ${ip}.xci]\n")
  endforeach()

  configure_file(run_behavioral.tcl ${test}_behav/run_behavioral.${test}.tcl)
  configure_file(bds.tcl ${test}_behav/bds.tcl)
  configure_file(files.tcl ${test}_behav/files.tcl)
  add_custom_command(
    OUTPUT ${test}_behav/export_sim/xsim/tb.sh
    COMMAND vivado -mode batch -nojournal -nolog -source ${COMMON_DIR} -source run_behavioral.${test}.tcl -tclargs ${test}
    # COMMAND sed -i 's/xvlog_opts=\"[^\"]*/& -L uvm/' ${CMAKE_CURRENT_BINARY_DIR}/${test}_behav/export_sim/xsim/tb.sh
    # COMMAND sed -i 's/xelab .*/& -L uvm/' ${CMAKE_CURRENT_BINARY_DIR}/${test}_behav/export_sim/xsim/tb.sh
    DEPENDS ${ips}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${test}_behav
  )
  # TODO: have a better way to avoid re-running and overriding existing waveforms
  # as it can take long to simulate
  add_custom_target(${test}_behav ALL DEPENDS ${test}_behav/export_sim/xsim/tb.sh)
  add_custom_target(run_${test}_behav ALL
    DEPENDS ${test}_behav
    COMMAND bash tb.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${test}_behav/export_sim/xsim/
  )
endfunction()

function(add_post_synth_sim test design bd_deps file_deps rmdcps)
  message(STATUS "Post implementation functional simulation")
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${test}_post_synth)
  set(rmdcps_locs "") # DCP file location
  set(rmvfiles "")   # Verilog files for the DCPs
  set(write_dcp_commands "")
  set(add_dcp_v_commands "")
  foreach(rm_name IN LISTS rmdcps)
    get_reconf_module_synth_dcp(NAME ${rm_name} RESULT rmdcp)
    get_reconf_module_target(NAME ${rm_name} RESULT rmv)
    list(APPEND rmdcps_locs ${rmdcp})
    message(STATUS "Added ${rmdcps_locs}")
    list(APPEND rmvfiles ${rmv})
    string(APPEND write_dcp_commands "open_checkpoint ${rmdcp}\nwrite_verilog -mode funcsim -force ${rmv}.v\nclose_design\n")
    string(APPEND add_dcp_v_commands "add_files -fileset sim_1 ${rmv}.v")
  endforeach()
  # set(rmdcp ${CMAKE_CURRENT_BINARY_DIR}/../rtl/RM_reconf_z3/RM_reconf_z3_synth.dcp)
  configure_file(run_impl.tcl ${test}_post_synth/run_impl.${test}.tcl)
  configure_file(bds.tcl ${test}_post_synth/bds.tcl)
  configure_file(files.tcl ${test}_post_synth/files.tcl)
  add_custom_command(
    OUTPUT ${test}_post_synth/export_sim/xsim/tb.sh ${test}_post_synth/${test}.v
    COMMAND vivado -mode batch -nojournal -nolog -source ${COMMON_DIR} -source run_impl.${test}.tcl -tclargs ${test}
    COMMAND sed -i 's/xvlog_opts=\"[^\"]*/& -L uvm/' ${CMAKE_CURRENT_BINARY_DIR}/${test}_post_synth/export_sim/xsim/tb.sh
    COMMAND sed -i 's/xelab .*/& -L uvm/' ${CMAKE_CURRENT_BINARY_DIR}/${test}_post_synth/export_sim/xsim/tb.sh
    DEPENDS ${rmvfiles}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${test}_post_synth
  )
  add_custom_target(${test}_post_synth ALL DEPENDS ${test}_post_synth/export_sim/xsim/tb.sh)
  add_custom_target(run_${test}_post_synth ALL
    DEPENDS ${test}_post_synth
    COMMAND bash tb.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${test}_post_synth/export_sim/xsim/
  )
endfunction()
