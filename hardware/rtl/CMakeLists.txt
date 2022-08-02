include (VivadoSynthesis.cmake)

# TODO: add code for hash

# ===== SystemC Modules
add_systemc_module(
  DESIGN sysc_z3test
  DESIGN_SOURCE ${PROJECT_SOURCE_DIR}/examples/llnl-examples/zfpsynth/zfp3/z3test.cpp
)
add_systemc_module(
  DESIGN sysc_z5test
  DESIGN_SOURCE ${PROJECT_SOURCE_DIR}/examples/llnl-examples/zfpsynth/zfp5/z5test.cpp
)
add_systemc_module(
  DESIGN sysc_z7test
  DESIGN_SOURCE ${PROJECT_SOURCE_DIR}/examples/llnl-examples/zfpsynth/zfp7/z7test.cpp
)
add_systemc_module(
  DESIGN sysc_hash
  DESIGN_SOURCE ${PROJECT_SOURCE_DIR}/examples/llnl-examples/modules/hash-unit/hash-unit.cpp
)

# ===== Reconfigurable Modules
add_reconf_module(DESIGN add_synth_test
  RECONF_PART "reconf_part_0" 
  RECONF_INST "inst_rp" 
  RTL add_synth_test.sv
  SYNTHESIS_OPTS "-directive AlternateRoutability" 
  )

add_reconf_module(DESIGN reconf_z3 
  RECONF_PART "reconf_part_0" 
  RECONF_INST "inst_rp" 
  RTL z3wrapper.sv
  SYSC_MODULE sysc_z3test
  # CONSTR "" 
  SYNTHESIS_OPTS "-directive AlternateRoutability" 
  # BDS "" 
  IPS axis_dwidth_32_64 axis_dwidth_64_32
  )
add_reconf_module(DESIGN reconf_z7
  RECONF_PART "reconf_part_0" 
  RECONF_INST "inst_rp" 
  RTL z7wrapper.sv
  SYSC_MODULE sysc_z7test
  # CONSTR "" 
  SYNTHESIS_OPTS "-directive AlternateRoutability" 
  # BDS "" 
  IPS axis_dwidth_32_64 axis_dwidth_64_32
  )
add_reconf_module(DESIGN reconf_z5
  RECONF_PART "reconf_part_0" 
  RECONF_INST "inst_rp" 
  RTL z3wrapper.sv
  SYSC_MODULE sysc_z5test
  # CONSTR "" 
  SYNTHESIS_OPTS "-directive AlternateRoutability" 
  # BDS "" 
  IPS axis_dwidth_32_64 axis_dwidth_64_32
  )



set(hash_ips axis_dwidth_32_136 axis_dwidth_32_256 axis_dwidth_256_32 axis_interconnect_1_2)
add_reconf_module(DESIGN reconf_hash
  RECONF_PART "reconf_part_0"
  RECONF_INST "inst_rp"
  RTL hashwrapper.sv
  SYSC_MODULE sysc_hash
  SYNTHESIS_OPTS "-directive AlternateRoutability" 
  IPS ${hash_ips}
  )

# ===== Reconfigurable Configurations

add_static_config(DESIGN top_hash
  RECONF_PART reconf_part_0
  RECONF_INST inst_rp
  RECONF_MOD  reconf_hash
  RTL         top_mcdma.sv
  SYNTHESIS_OPTS "-directive  AlternateRoutability -verilog_define ${BOARD}"
  BDS         bd
  CONSTR      ${BOARD}.xdc 
  TOP_MOD     top
)

add_static_config(DESIGN top_z7_static
  RECONF_PART reconf_part_0
  RECONF_INST inst_rp
  RECONF_MOD  reconf_z7
  RTL         top_mcdma.sv
  SYNTHESIS_OPTS "-directive  AlternateRoutability -verilog_define ${BOARD}"
  BDS         bd
  CONSTR      ${BOARD}.xdc 
  TOP_MOD     top
)

add_config_from_static(DESIGN top_z3
  RECONF_PART   reconf_part_0
  RECONF_INST   inst_rp
  RECONF_MOD    reconf_z3
  STATIC_DESIGN top_hash  
  # CONSTR        vu9p.xdc inst_rp.xdc
)
add_config_from_static(DESIGN top_z5
  RECONF_PART   reconf_part_0
  RECONF_INST   inst_rp
  RECONF_MOD    reconf_z5
  STATIC_DESIGN top_hash  
  # CONSTR        vu9p.xdc inst_rp.xdc
)
add_config_from_static(DESIGN top_z7
  RECONF_PART   reconf_part_0
  RECONF_INST   inst_rp
  RECONF_MOD    reconf_z7
  STATIC_DESIGN top_hash  
  # CONSTR        vu9p.xdc inst_rp.xdc
)
