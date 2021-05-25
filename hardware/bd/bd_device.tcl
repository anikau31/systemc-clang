# select block design based on device
if { $bp eq "" } {
  source bd_ultra96v2.tcl
} elseif { $bp eq "" && $part eq "" } {
  source bd_pynq_z1.tcl
} else {
  error "Unsupported configuration. Supported devices are PYNQ-Z1 and Ultra96V2, refer to the documentation for more information"
}
