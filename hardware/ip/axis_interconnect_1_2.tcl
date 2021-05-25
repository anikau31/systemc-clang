set ip_name "axis_interconnect_1_2"
create_project -in_memory -part $part
create_ip -name axis_interconnect -vendor xilinx.com -library ip -version 1.1 -module_name $ip_name
set_property -dict [list CONFIG.Component_Name {$ip_name} CONFIG.C_NUM_MI_SLOTS {2} CONFIG.SWITCH_TDATA_NUM_BYTES {4} CONFIG.HAS_TSTRB {false} CONFIG.HAS_TKEEP {true} CONFIG.HAS_TID {false} CONFIG.HAS_TUSER {false} CONFIG.C_M00_AXIS_REG_CONFIG {1} CONFIG.C_S00_AXIS_REG_CONFIG {0} CONFIG.S00_AXIS_FIFO_MODE {0_(Disabled)} CONFIG.C_M01_AXIS_REG_CONFIG {1} CONFIG.C_SWITCH_TID_WIDTH {1} CONFIG.HAS_TDEST {true} CONFIG.C_SWITCH_TDEST_WIDTH {4} CONFIG.SWITCH_TUSER_BITS_PER_BYTE {1} CONFIG.C_SWITCH_NUM_CYCLES_TIMEOUT {0} CONFIG.M00_AXIS_TDATA_NUM_BYTES {4} CONFIG.S00_AXIS_TDATA_NUM_BYTES {4} CONFIG.M01_AXIS_TDATA_NUM_BYTES {4} CONFIG.C_M00_AXIS_BASETDEST {0x00000000} CONFIG.C_M00_AXIS_HIGHTDEST {0x00000000} CONFIG.C_M01_AXIS_BASETDEST {0x00000001} CONFIG.C_M01_AXIS_HIGHTDEST {0x00000001} CONFIG.M01_S00_CONNECTIVITY {true} CONFIG.C_SWITCH_MI_REG_CONFIG {1}  ] [get_ips $ip_name]
generate_target -force all [get_files $ip_name.xci]
set_property generate_synth_checkpoint true [get_files $ip_name.xci]
synth_ip -force [get_files $ip_name.xci]
