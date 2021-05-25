
open_checkpoint ${design}_impl.dcp
report_utilization -file ${design}_util.txt -hierarchical -hierarchical_depth 2 -cells inst_rp
report_utilization -file ${design}_util_overall.txt -hierarchical
