# open dcp
open_checkpoint [lindex $argv 0]
# report utilization
report_utilization -hierarchical -cells [get_cells u96v2_sbc_base_i/ma] -file moving-average-util.txt