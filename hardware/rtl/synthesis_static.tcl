create_project -in_memory -part $part
set_property board_part $bp [current_project]

# RTL Design
add_files { ${rtl_files_space_sep} }

# Block design wrapper files
add_files { ${bd_wrappers_space_sep} }

# IP wrapper files
add_files { ${ip_wrappers_space_sep} }

# XDC
add_files { ${constraint_files_space_sep} }

# DCP files
add_files { ${dcp_files_space_sep} }

synth_design -part $part -top ${top_module} ${synthesis_options}

# NOTE: the output directory is one level above
write_checkpoint -force ${design}_synth.dcp
