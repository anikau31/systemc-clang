include (VivadoSynthesis.cmake)

# TODO: add code for hash

# ===== SystemC Modules
# add_systemc_generated_module(
#   DESIGN sysc_z3test_d64
#   DESIGN_SOURCE ${PROJECT_SOURCE_DIR}/fccm-case-studies/zfpsynth/zfp3/z3test.cpp
# )
# add_systemc_generated_module(
#   DESIGN sysc_z3test_d32
#   DESIGN_SOURCE ${PROJECT_SOURCE_DIR}/fccm-case-studies/zfpsynth/zfp3-fp32/z3test.cpp
# )
# add_systemc_generated_module(
#   DESIGN sysc_z7test_d64
#   DESIGN_SOURCE ${PROJECT_SOURCE_DIR}/fccm-case-studies/zfpsynth/zfp7/z7test.cpp
# )
# add_systemc_generated_module(
#   DESIGN sysc_z7test_d32
#   DESIGN_SOURCE ${PROJECT_SOURCE_DIR}/fccm-case-studies/zfpsynth/zfp7_fp32_2d/z7test.cpp
# )


# ===== Reconfigurable Modules
add_reconf_module(DESIGN add_synth_test
  RECONF_PART "reconf_part_0" 
  RECONF_INST "inst_rp" 
  RTL add_synth_test.sv
  SYNTHESIS_OPTS "-directive AlternateRoutability" 
  )

add_reconf_module(DESIGN reconf_z3_d64
  RECONF_PART "reconf_part_0" 
  RECONF_INST "inst_rp" 
  RTL z3wrapper.sv 
  RAW_RTL ${PROJECT_SOURCE_DIR}/fccm-case-studies/zfpsynth/zfp3/z3test_hdl.txt.sv
  # SYSC_MODULE sysc_z3test_d64
  # CONSTR "" 
  SYNTHESIS_OPTS "-directive AlternateRoutability" 
  # BDS "" 
  IPS axis_dwidth_32_64 axis_dwidth_64_32
  )
add_reconf_module(DESIGN reconf_z3_d32
  RECONF_PART "reconf_part_0" 
  RECONF_INST "inst_rp" 
  RTL z3wrapper_32.sv 
  RAW_RTL ${PROJECT_SOURCE_DIR}/fccm-case-studies/zfpsynth/zfp3-fp32/z3test_hdl.txt.sv
  # SYSC_MODULE sysc_z3test_d32
  # CONSTR "" 
  SYNTHESIS_OPTS "-directive AlternateRoutability" 
  # BDS "" 
  IPS axis_dwidth_32_32
  )
add_reconf_module(DESIGN reconf_z7_d64
  RECONF_PART "reconf_part_0" 
  RECONF_INST "inst_rp" 
  RTL z7wrapper.sv 
  RAW_RTL ${PROJECT_SOURCE_DIR}/fccm-case-studies/zfpsynth/zfp7/z7test_hdl.txt.sv
  # SYSC_MODULE sysc_z7test_d64
  # CONSTR "" 
  SYNTHESIS_OPTS "-directive AlternateRoutability" 
  # BDS "" 
  IPS axis_dwidth_32_64 axis_dwidth_64_32
  )
add_reconf_module(DESIGN reconf_z7_d32
  RECONF_PART "reconf_part_0" 
  RECONF_INST "inst_rp" 
  RTL z7wrapper_32.sv 
  RAW_RTL ${PROJECT_SOURCE_DIR}/fccm-case-studies/zfpsynth/zfp7_fp32_2d/z7test_hdl.txt.sv
  # SYSC_MODULE sysc_z7test_d32
  # CONSTR "" 
  SYNTHESIS_OPTS "-directive AlternateRoutability" 
  # BDS "" 
  IPS axis_dwidth_32_32
  )

# ===== Reconfigurable Configurations

add_static_config(DESIGN top_z7_d64
  RECONF_PART reconf_part_0
  RECONF_INST inst_rp
  RECONF_MOD  reconf_z7_d64
  RTL         top_mcdma.sv
  SYNTHESIS_OPTS "-directive  AlternateRoutability -verilog_define ${BOARD}"
  BDS         bd
  CONSTR      ${BOARD}.xdc 
  TOP_MOD     top
)
add_static_config(DESIGN top_z7_d32
  RECONF_PART reconf_part_0
  RECONF_INST inst_rp
  RECONF_MOD  reconf_z7_d32
  RTL         top_mcdma.sv
  SYNTHESIS_OPTS "-directive  AlternateRoutability -verilog_define ${BOARD}"
  BDS         bd
  CONSTR      ${BOARD}.xdc 
  TOP_MOD     top
)

add_static_config(DESIGN top_z3_d64
  RECONF_PART reconf_part_0
  RECONF_INST inst_rp
  RECONF_MOD  reconf_z3_d64
  RTL         top_mcdma.sv
  SYNTHESIS_OPTS "-directive  AlternateRoutability -verilog_define ${BOARD}"
  BDS         bd
  CONSTR      ${BOARD}.xdc 
  TOP_MOD     top
)
add_static_config(DESIGN top_z3_d32
  RECONF_PART reconf_part_0
  RECONF_INST inst_rp
  RECONF_MOD  reconf_z3_d32
  RTL         top_mcdma.sv
  SYNTHESIS_OPTS "-directive  AlternateRoutability -verilog_define ${BOARD}"
  BDS         bd
  CONSTR      ${BOARD}.xdc 
  TOP_MOD     top
)
