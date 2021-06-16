-define VIVADO_SIM
-define ${BOARD}

# use zynq vip instead of the real zynq
${PROJECT_SOURCE_DIR}/verif/tests/test_z3.sv
${PROJECT_SOURCE_DIR}/verif/tb/tb.sv

${PROJECT_SOURCE_DIR}/rtl/z3wrapper.sv
${PROJECT_SOURCE_DIR}/rtl/z3test_hdl.txt.sv
${PROJECT_SOURCE_DIR}/rtl/top.sv
${PROJECT_SOURCE_DIR}/rtl/shell_wrapper.sv
