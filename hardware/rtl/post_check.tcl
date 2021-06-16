create_project -in_memory -part $part
open_checkpoint "${design}_impl.dcp"

puts "Performing timing checks for ${design} implementation"

if [expr {[get_property SLACK [get_timing_paths -delay_type min_max]] < 0}] { 
  error "ERROR: Timing failed" 
} else {
  set slacks [get_property SLACK [get_timing_paths -delay_type min_max]]
  puts "SLACKS:  $slacks"
}


