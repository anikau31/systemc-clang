# mkdir -p moving-average-d16-w64
# mkdir -p moving-average-d64-w64
# mkdir -p moving-average-d16-w16
# mkdir -p moving-average-d64-w16

# for a in moving-average-d16-w64 \
# moving-average-d64-w64 \
# moving-average-d16-w16 \
# moving-average-d64-w16 ; do
#   mkdir -p $a
#   cp moving-average.cpp $a/
#   cp moving-average.h $a/
# done

# for a in mm-n4-d8 mm-n8-d8 mm-n16-d8; do
#   mkdir -p $a
#   cp /home/allen/working/llnl-script-debug/ece327/lab4/{mem_read_A.sv,mem_read_B.sv,mem_write.sv,s2mm.sv,mm2s.sv,mem.v,mm_axi.v,mm.sv} $a/
#   cp /home/allen/working/llnl-script-debug/ece327/lab4/systemc/systolic_array.h $a/
#   cp /home/allen/working/llnl-script-debug/ece327/lab4/systemc/dut.cpp $a/mm.cpp
# done


# for a in mm-manual-n4-d8 mm-manual-n8-d8 mm-manual-n16-d8; do
#   mkdir -p $a
#   cp /home/allen/working/llnl-script-debug/ece327/lab4.bak.bak/{mem_read_A.sv,mem_read_B.sv,mem_write.sv,s2mm.sv,mm2s.sv,mem.v,mm_axi.v,mm.sv,systolic.sv,pe.v,counter.v,control.v} $a/
# done

# for a in moving-average-shift-d16-w64 \
# moving-average-shift-d64-w64 \
# moving-average-shift-d16-w16 \
# moving-average-shift-d64-w16 ; do
# 	# mkdir -p $a
# 	# b=${a/-shift/}
# 	# mkdir -p $a
# 	# cp $b/moving-average.cpp ./$a/
# 	# cp /home/allen/mount/working/systemc-clang/examples/llnl-examples/modules/moving-average-shift/moving-average.h ./$a/
#   # rm ./$a/moving-average.h
#   cp constr.xdc $a
# done

for a in moving-average-pipe-d16-w64 \
moving-average-pipe-d64-w64 \
moving-average-pipe-d16-w16 \
moving-average-pipe-d64-w16 ; do
	# mkdir -p $a
	b=${a/-pipe/}
	mkdir -p $a
	cp $b/moving-average.cpp ./$a/
	cp /home/allen/mount/working/systemc-clang/examples/llnl-examples/modules/moving-average-pipe/moving-average.h ./$a/
  rm ./$a/moving-average.h
  cp constr.xdc $a
done
