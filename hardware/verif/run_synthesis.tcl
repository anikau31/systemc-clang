# post-synthesis simulation
# sourced common
set test_name [lindex $argv 0]

create_project -in_memory -part $part

source bds.tcl
source files.tcl

foreach bd $::bd_deps {
  # source
  source ${PROJECT_SOURCE_DIR}/bd/$bd.tcl
  generate_target simulation [get_files $bd.bd]
  add_files -fileset sim_1 ${CMAKE_CURRENT_BINARY_DIR}/${test}/.srcs/sources_1/bd/shell/hdl/$bd\_wrapper.v
}

foreach f $::files {
  add_files -fileset sim_1 ${PROJECT_SOURCE_DIR}/rtl/$f.sv
}
add_files -fileset sim_1 ${PROJECT_SOURCE_DIR}/verif/tb/tb.sv
add_files -fileset sim_1 ${PROJECT_SOURCE_DIR}/verif/tests/$test_name.sv

set_property -name {xelab.more_options}   -value {-L uvm} -objects [get_filesets sim_1]
set_property -name {xsim.elaborate.xelab.more_option} -value {-L uvm} -objects [get_filesets sim_1]
set_property top tb [get_filesets sim_1]

export_simulation -simulator xsim -use_ip_compiled_libs -force

