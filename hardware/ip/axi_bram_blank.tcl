set ip_name "axi_bram_blank"
create_project -in_memory -part $part
create_ip -name axi_bram_ctrl -vendor xilinx.com -library ip -version 4.1 -module_name $ip_name
set_property -dict [list CONFIG.DATA_WIDTH {64} CONFIG.ECC_TYPE {0} CONFIG.USE_ECC {0} CONFIG.Component_Name {axi_bram_blank} CONFIG.BMG_INSTANCE {INTERNAL}] [get_ips $ip_name]
generate_target -force all [get_files $ip_name.xci]
set_property generate_synth_checkpoint true [get_files $ip_name.xci]
synth_ip -force [get_files $ip_name.xci]
# write_checkpoint -force $ip_name.dcp
# synth_design -part $part -mode out_of_context

