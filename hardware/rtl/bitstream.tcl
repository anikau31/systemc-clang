create_project -in_memory -part $part
open_checkpoint "${design}_impl.dcp"

write_bitstream -force -bin_file ${design}
write_debug_probes -force ${design}.ltx
