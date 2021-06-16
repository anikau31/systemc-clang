# sourced common.tcl
# sourced block design tcl
set name [lindex $argv 0]
set bdtcl [lindex $argv 1]

create_project -in_memory -part $part
set_property board_part $bp [current_project]

source $bdtcl
generate_target all [get_files $name.bd ]
synth_design -top [lindex $argv 0] -mode out_of_context -directive  AlternateRoutability

write_checkpoint -force $name.dcp
