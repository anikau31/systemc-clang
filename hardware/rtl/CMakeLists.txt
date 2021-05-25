include (VivadoSynthesis.cmake)

# Conversion from cpp files to txt files
# Calling python script to generate the design, use the detected python
# TODO: connect this to the original project
set(SYSTEMC_CLANG_FLAGS "-I ../systemc/include -x c++ -w -c -std=c++14 -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DRVD")

# TODO: wrap the commands into a function
add_custom_command(OUTPUT ${SYNTH_ROOT_DIR}/rtl/z3test_hdl.txt.sv
  COMMAND "${PYTHON_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/plugins/hdl/systemc-clang.py"
  "${PROJECT_SOURCE_DIR}/examples/llnl-examples/zfpsynth/zfp3/z3test.cpp" 
  "--" ${SYSTEMC_CLANG_FLAGS} "---" "-o" "${SYNTH_ROOT_DIR}/rtl/z3test_hdl.txt.sv")
add_custom_target(z3test_hdl DEPENDS ${SYNTH_ROOT_DIR}/rtl/z3test_hdl.txt.sv)

# TODO: add code for hash


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
  RTL z3test_hdl.txt.sv z3wrapper.sv
  # CONSTR "" 
  SYNTHESIS_OPTS "-directive AlternateRoutability" 
  # BDS "" 
  IPS axis_dwidth_32_64 axis_dwidth_64_32
  )

set(hash_ips axis_dwidth_32_136 axis_dwidth_32_256 axis_dwidth_256_32 axis_interconnect_1_2)
add_reconf_module(DESIGN reconf_hash
  RECONF_PART "reconf_part_0"
  RECONF_INST "inst_rp"
  RTL hash_unit_hdl.txt.sv hashwrapper.sv
  SYNTHESIS_OPTS "-directive AlternateRoutability" 
  IPS ${hash_ips}
  )

# ===== Reconfigurable Configurations

add_static_config(DESIGN top_mcdma_static
  RECONF_PART reconf_part_0
  RECONF_INST inst_rp
  RECONF_MOD  reconf_hash
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
  STATIC_DESIGN top_mcdma_static  
  # CONSTR        vu9p.xdc inst_rp.xdc
)


