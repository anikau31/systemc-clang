# sourced common

# NOTE: the builtin script ignores xvlog_opts, we modify the script to add those options
source ${PROJECT_SOURCE_DIR}/tcl/projutils/projutils.tcl
source ${PROJECT_SOURCE_DIR}/tcl/projutils/common/utils.tcl
source ${PROJECT_SOURCE_DIR}/tcl/projutils/export_simulation.tcl
set test_name [lindex $argv 0]

create_project -in_memory -part $part

source bds.tcl
source files.tcl

foreach bd $::bd_deps {
  # source
  source ${PROJECT_SOURCE_DIR}/bd/$bd\_${BOARD}.tcl
  generate_target simulation [get_files $bd.bd]
  add_files -fileset sim_1 ${CMAKE_CURRENT_BINARY_DIR}/${test}_behav/.srcs/sources_1/bd/$bd/hdl/$bd\_wrapper.v
}

foreach f $::files {
  add_files -fileset sim_1 ${PROJECT_SOURCE_DIR}/rtl/$f.sv
}
add_files -fileset sim_1 ${PROJECT_SOURCE_DIR}/verif/tb/tb.sv
add_files -fileset sim_1 ${PROJECT_SOURCE_DIR}/verif/tests/$test_name.sv

# add ips
${add_ip_sim_command}

# these set_property commands are likely ignored by vivado when exporting simulation
set_property -name {xelab.more_options}   -value {-L uvm} -objects [get_filesets sim_1]
set_property -name {xsim.elaborate.xelab.more_option} -value {-L uvm} -objects [get_filesets sim_1]
set_property top tb [get_filesets sim_1]

::tclapp::xilinx::projutils::ext_export_simulation -extra_xvlog_opt "-L uvm --define ${BOARD}" -extra_xelab_opt "-L uvm" -simulator xsim -of_objects [get_fileset sim_1] -use_ip_compiled_libs -force

