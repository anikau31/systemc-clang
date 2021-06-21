-define VIVADO_SIM
-define ${BOARD}

# use zynq vip instead of the real zynq
${PROJECT_SOURCE_DIR}/verif/tests/test_hash.sv
${PROJECT_SOURCE_DIR}/verif/tb/tb.sv

${PROJECT_SOURCE_DIR}/rtl/hashwrapper.sv
${PROJECT_SOURCE_DIR}/rtl/hash_unit_hdl.txt.sv
${PROJECT_SOURCE_DIR}/rtl/top.sv
${PROJECT_SOURCE_DIR}/rtl/shell_wrapper.sv
# ${SYNTH_ROOT_DIR}/bd/bd/.srcs/sources_1/bd/bd/hdl/bd_wrapper.v
