set ip_name "axis_dwidth_256_32"
create_project -in_memory -part $part
create_ip -name axis_dwidth_converter -vendor xilinx.com -library ip -version 1.1 -module_name $ip_name
set_property -dict [list CONFIG.S_TDATA_NUM_BYTES {32} CONFIG.M_TDATA_NUM_BYTES {4} CONFIG.Component_Name {$ip_name}] [get_ips $ip_name]
generate_target -force all [get_files $ip_name.xci]
set_property generate_synth_checkpoint true [get_files $ip_name.xci]
synth_ip -force [get_files $ip_name.xci]
