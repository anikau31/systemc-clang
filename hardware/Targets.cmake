# get the reconfigurable module dcp
function(get_reconf_module_dir)
  set(oneValueArgs NAME RESULT)
  cmake_parse_arguments(RM "" "${oneValueArgs}" "" ${ARGN})
  set(${RM_RESULT} RM_${RM_NAME} PARENT_SCOPE)
endfunction()
function(get_reconf_module_target)
  set(oneValueArgs NAME RESULT)
  cmake_parse_arguments(RM "" "${oneValueArgs}" "" ${ARGN})
  get_reconf_module_dir(
    NAME ${RM_NAME}
    RESULT dir)

  set(${RM_RESULT} ${dir}_synth PARENT_SCOPE)
endfunction()
function(get_reconf_module_synth_dcp)
  set(oneValueArgs NAME RESULT)
  cmake_parse_arguments(RM "" "${oneValueArgs}" "" ${ARGN})
  get_reconf_module_target(
    NAME ${RM_NAME}
    RESULT target)
  get_reconf_module_dir(
    NAME ${RM_NAME}
    RESULT dir)
  set(${RM_RESULT} ${SYNTH_ROOT_DIR}/${RTL_DIR}/${dir}/${target}.dcp PARENT_SCOPE)
endfunction()
