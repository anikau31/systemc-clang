create_project -in_memory -part $part
set_property board_part $bp [current_project]

# DCP files
# add_files { ${dcp_files_space_sep} }

# Main design
open_checkpoint "${design}_synth.dcp"

# XDC
# read_xdc { ${constr_files} }
${add_xdc_command}

${reconf_command}

opt_design
write_checkpoint -force ${design}_post_opt.dcp
place_design
route_design


write_checkpoint -force ${design}_impl.dcp

# Make partitions black boxes
${blackboxing_command}

# Lock design with routing for other configurations
${lock_design_if_reconf}
