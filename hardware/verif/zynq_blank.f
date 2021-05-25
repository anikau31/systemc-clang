-define VIVADO_SIM
-define {BOARD}

# use zynq vip instead of the real zynq
${PROJECT_SOURCE_DIR}/verif/tests/test_zynq_blank.sv
${PROJECT_SOURCE_DIR}/verif/tb/tb.sv

${PROJECT_SOURCE_DIR}/rtl/top.sv
${PROJECT_SOURCE_DIR}/rtl/shell_wrapper.sv
