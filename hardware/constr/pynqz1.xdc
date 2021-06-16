# reconfigurable partition
create_pblock inst_rp
add_cells_to_pblock [get_pblocks inst_rp] [get_cells -quiet [list inst_rp]]
resize_pblock [get_pblocks inst_rp] -add {CLOCKREGION_X1Y0:CLOCKREGION_X1Y2 }
set_property IS_SOFT FALSE [get_pblocks inst_rp]
