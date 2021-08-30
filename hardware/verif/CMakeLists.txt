include(VivadoSimulation.cmake)


set(zynq_bds shell)
set(file_deps "top add")

# add_behavioral_sim(test_blank blank)
add_behavioral_sim               (test_zynq_blank zynq_blank ${zynq_bds} ${file_deps} "" "")
# add_behavioral_sim               (test_z3         zynq_z3    ${zynq_bds} "top z3wrapper z3test_hdl.txt" "")
set(z3_ips axis_dwidth_32_64 axis_dwidth_64_32)
add_behavioral_sim               (test_z3         zynq_z3    "bd" "top_mcdma z3wrapper" "sysc_z3test" "${z3_ips}")
set(hash_ips axis_dwidth_32_136 axis_dwidth_32_256 axis_dwidth_256_32 axis_interconnect_1_2)
add_behavioral_sim               (test_hash       zynq_hash  "bd"        "top_mcdma hashwrapper" "sysc_hash" "${hash_ips}")
# add_post_synthesis_functional_sim(test_zynq_blank_post_synth)
add_post_synth_sim(test_z3   zynq_z3   ${zynq_bds} "top" "reconf_z3")
add_post_synth_sim(test_hash zynq_hash "bd"        "top_mcdma"  "reconf_hash")
# add_post_impl_sim                (test_zynq_blank_post_impl)
