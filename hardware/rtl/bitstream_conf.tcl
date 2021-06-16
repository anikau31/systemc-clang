create_project -in_memory -part $part
open_checkpoint "${design}_impl.dcp"

pr_verify ${static_dcp} ${design}_impl.dcp

write_bitstream -force ${design}.bit
write_debug_probes -force ${design}.ltx
