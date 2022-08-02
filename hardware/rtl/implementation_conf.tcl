
open_checkpoint ${locked_dcp}

${add_reconf_command}

${add_xdc_command}

opt_design
place_design -directive Auto
route_design

write_checkpoint -force ${design}_impl.dcp
