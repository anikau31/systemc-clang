SYSTEMC_CLANG_EXAMPLES:=${SYSTEMC_CLANG}/fccm-case-studies/
SC		 :=${SYSTEMC_CLANG}/scripts/sc
VIVADO ?= vivado
VPP 	 ?= v++
# # Example platform path
# /home/allen/mount2/vitis_platforms/u96v2_sbc_base/u96v2_sbc_base.xpfm
PLATFORM:=${ULTRA96_V2_PLATFORM}
VPP_FLAGS=--save-temps --temp_dir temp --log_dir log --report_dir report

MA := moving-average-d16-w16 moving-average-d64-w16 moving-average-d16-w64 moving-average-d64-w64
MA_SVS := $(addsuffix /moving-average_hdl.txt.sv, $(MA))
MA_TCLS := $(addsuffix /moving-average.tcl, $(MA))
MA_XOS := $(addsuffix /moving-average.xo, $(MA))
MA_XCLBINS := $(addsuffix /moving-average.xclbin, $(MA))
MA_UTILS := $(addsuffix /moving-average-util.txt, $(MA))
MA_UTILS_FILTERED := $(addsuffix /moving-average-util-filtered.txt, $(MA))

MAP := moving-average-pipe-d16-w16 moving-average-pipe-d64-w16 moving-average-pipe-d16-w64 moving-average-pipe-d64-w64
MAP_SVS := $(addsuffix /moving-average_hdl.txt.sv, $(MA))
MAP_TCLS := $(addsuffix /moving-average.tcl, $(MA))
MAP_XOS := $(addsuffix /moving-average.xo, $(MA))
MAP_XCLBINS := $(addsuffix /moving-average.xclbin, $(MA))
MAP_UTILS := $(addsuffix /moving-average-util.txt, $(MA))
MAP_UTILS_FILTERED := $(addsuffix /moving-average-util-filtered.txt, $(MA))


DIV := div-8 div-16 div-32 div-64
DIV_SVS := $(addsuffix /divider_hdl.txt.sv, $(DIV))
DIV_TCLS := $(addsuffix /divider.tcl, $(DIV))
DIV_XOS := $(addsuffix /divider.xo, $(DIV))
DIV_XCLBINS := $(addsuffix /divider.xclbin, $(DIV))
DIV_UTILS := $(addsuffix /divider-util.txt, $(DIV))
DIV_UTILS_FILTERED := $(addsuffix /divider-util-filtered.txt, $(DIV))

MAS := moving-average-shift-d16-w16 moving-average-shift-d64-w16 moving-average-shift-d16-w64 moving-average-shift-d64-w64
MAS_SVS := $(addsuffix /moving-average_hdl.txt.sv, $(MA))
MAS_TCLS := $(addsuffix /moving-average.tcl, $(MA))
MAS_XOS := $(addsuffix /moving-average.xo, $(MA))
MAS_XCLBINS := $(addsuffix /moving-average.xclbin, $(MA))
MAS_UTILS := $(addsuffix /moving-average-util.txt, $(MA))
MAS_UTILS_FILTERED := $(addsuffix /moving-average-util-filtered.txt, $(MA))

MM := mm-n8-d8 mm-n16-d8
MM_SVS := $(addsuffix /mm_hdl.txt.sv, $(MM))
MM_TCLS := $(addsuffix /mm.tcl, $(MM))
MM_XOS := $(addsuffix /mm.xo, $(MM))
MM_XCLBINS := $(addsuffix /mm.xclbin, $(MM))
MM_UTILS := $(addsuffix /mm-util.txt, $(MM))
MM_UTILS_FILTERED := $(addsuffix /mm-util-filtered.txt, $(MM))


# MMM - MM manual
MMM := mm-manual-n8-d8 mm-manual-n16-d8
MMM_TCLS := $(addsuffix /mmm.tcl, $(MMM))
MMM_XOS := $(addsuffix /mmm.xo, $(MMM))
MMM_XCLBINS := $(addsuffix /mmm.xclbin, $(MMM))
MMM_UTILS := $(addsuffix /mmm-util.txt, $(MMM))
MMM_UTILS_FILTERED := $(addsuffix /mmm-util-filtered.txt, $(MMM))

tcls: $(MA_TCLS) $(MAS_TCLS) $(MAP_TCLS) $(MM_TCLS) $(MMM_TCLS) $(DIV_TCLS)
xos: $(MA_XOS) $(MAS_XOS) $(MAP_XOS) $(MM_XOS) $(MMM_XOS) $(DIV_XOS)
svs: $(MA_SVS) $(MAS_SVS) $(MAP_SVS) $(MM_SVS) $(DIV_SVS)
xclbins: $(MM_XCLBINS) $(MA_XCLBINS) $(MAS_XCLBINS) $(MAP_XCLBINS)  $(MMM_XCLBINS) $(DIV_XCLBINS)
utils: $(MA_UTILS_FILTERED) $(MAS_UTILS_FILTERED) $(MAP_UTILS_FILTERED) $(MM_UTILS_FILTERED) $(MMM_UTILS_FILTERED) $(DIV_UTILS_FILTERED)

xo_files:=$(MA_XOS) $(MAS_XOS) $(MAP_XOS) $(MM_XOS) $(MMM_XOS) $(DIV_XOS)
# manual matrix-multiplication should not need to be cleaned
sv_files:=$(MA_SVS) $(MAS_SVS) $(MAP_SVS) $(MM_SVS) $(DIV_SVS)

clean:
	rm -rf $(xo_files)
	rm -rf $(sv_files)
	find . -name ".Xil" | xargs -I{} rm -rf {}
	find . -name ".ipcache" | xargs -I{} rm -rf {}
	find . -name "package" | xargs -I{} rm -rf {}
	find . -name "report" | xargs -I{} rm -rf {}
	find . -name "log" | xargs -I{} rm -rf {}
	find . -name "tmp_*" | xargs -I{} rm -rf {}
	find . -name "temp" | xargs -I{} rm -rf {}
	find . -name "*.xo" | xargs -I{} rm -rf {}
	find . -name "*.log" | xargs -I{} rm -rf {}
	find . -name "*.txt" | xargs -I{} rm -rf {}
	find . -name "*.jou" | xargs -I{} rm -rf {}
	find . -name "*.link_summary" | xargs -I{} rm -rf {}
	find . -name "*.info" | xargs -I{} rm -rf {}
	find . -name "*.xclbin" | xargs -I{} rm -rf {}
	find . -name "*.xml" | xargs -I{} rm -rf {}
	find . -mindepth 2 -name "*.tcl"  | xargs -I{} rm -rf {}

starter.xo: starter.cpp
	v++ ${VPP_FLAGS} -g -k starter -c -o $@ --platform ${PLATFORM} $^

mms.xo: mms.cpp
	v++ ${VPP_FLAGS} -k mms -c -o $@ --platform ${PLATFORM} $^

div_starter.xo: div_starter.cpp
	v++ ${VPP_FLAGS} -k mms -c -o $@ --platform ${PLATFORM} $^


%/moving-average.tcl: moving-average.yaml generate_package_script.py
	cd $* && python ../generate_package_script.py ../moving-average.yaml -o $(notdir $@)
%/divider.tcl: divider.yaml generate_package_script.py
	cd $* && python ../generate_package_script.py ../divider.yaml -o $(notdir $@)

%/mm.tcl: mm.yaml generate_package_script.py
	cd $* && python ../generate_package_script.py ../mm.yaml -o $(notdir $@)
%/mmm.tcl: mmm.yaml generate_package_script.py
	cd $* && python ../generate_package_script.py ../mmm.yaml -o $(notdir $@)

# MA-Div-Sub
moving-average-pipe-%/moving-average_hdl.txt.sv: moving-average-pipe-%/moving-average.cpp
	# ./sc $^ -I${SYSTEMC_CLANG_EXAMPLES}/moving-average-pipe/
	mv moving-average-pipe-$*/moving-average_hdl.txt.v $@
	# bash post-processing.sh $@

# MA-Shift
moving-average-shift-%/moving-average_hdl.txt.sv: moving-average-shift-%/moving-average.cpp
	# ./sc $^ -I${SYSTEMC_CLANG_EXAMPLES}/moving-average-shift/
	mv moving-average-shift-$*/moving-average_hdl.txt.v $@
	bash post-processing.sh $@

# MA-Div
%/moving-average_hdl.txt.sv: %/moving-average.cpp
	# ./sc $^ -I${SYSTEMC_CLANG_EXAMPLES}/moving-average/
	mv $*/moving-average_hdl.txt.v $@
	bash post-processing.sh $@

%/divider_hdl.txt.sv: %/divider.cpp
	# ./sc $^ -I${SYSTEMC_CLANG_EXAMPLES}/divider-thread/
	mv $*/divider_hdl.txt.v $@

%/mm_hdl.txt.sv: %/mm.cpp
	# ./sc $^ -I${SYSTEMC_CLANG_EXAMPLES}/systolic/
	mv $*/mm_hdl.txt.v $@

# moving-average.tcl: moving-average.yaml
# 	python generate_package_script.py moving-average.yaml -o moving-average.tcl

%/moving-average.xo: %/moving-average.tcl %/moving-average_hdl.txt.sv
	cd $* && vivado -mode batch -nojournal -nolog -source moving-average.tcl
%/divider.xo: %/divider.tcl %/divider_hdl.txt.sv
	cd $* && vivado -mode batch -nojournal -nolog -source divider.tcl


%/mm.xo: %/mm.tcl %/mm_hdl.txt.sv
	cd $* && vivado -mode batch -nojournal -nolog -source mm.tcl

%/mmm.xo: %/mmm.tcl
	cd $* && vivado -mode batch -nojournal -nolog -source mmm.tcl
# moving-average.xo: moving-average.tcl
# 	vivado -mode batch -nojournal -nolog -source moving-average.tcl

# moving-average.xclbin: moving-average.xo ../starter.xo ../connect.cfg
# 	v++ ${VPP_FLAGS} --report_level 2 --target hw --platform ${PLATFORM} --link -o $@  moving_average_sc_module_1.xo starter.xo --config connect.cfg


moving-average-pipe-%/moving-average.xclbin: moving-average-pipe-%/moving-average.xo starter.xo connect.cfg
	cd moving-average-pipe-$* && v++ ${VPP_FLAGS} --report_level 2 --target hw --platform ${PLATFORM} --link -o moving-average.xclbin moving-average.xo ../starter.xo --config ../connect.cfg

moving-average-shift-%/moving-average.xclbin: moving-average-shift-%/moving-average.xo starter.xo connect-shift.cfg
	cd moving-average-shift-$* && v++ ${VPP_FLAGS} --report_level 2 --target hw --platform ${PLATFORM} --link -o moving-average.xclbin moving-average.xo ../starter.xo --config ../connect-shift.cfg

%/moving-average.xclbin: %/moving-average.xo starter.xo connect-div.cfg
	cd $* && v++ \
		${VPP_FLAGS} --report_level 2 --target hw --platform ${PLATFORM} --link -o moving-average.xclbin moving-average.xo ../starter.xo --config ../connect-div.cfg

#		-g \
#		--debug.chipscope starter_1 \
#		--debug.chipscope starter_1:data_strm \
#		--debug.chipscope starter_1:avg_out \

%/divider.xclbin: %/divider.xo div_starter.xo connect-divider.cfg
	cd $* && v++ \
		-g \
		--debug.chipscope mms_1 \
		--debug.chipscope mms_1:x \
		--debug.chipscope mms_1:y \
		--debug.chipscope mms_1:z \
		--debug.chipscope ma:dividend \
		--debug.chipscope ma:divisor \
		--debug.chipscope ma:quotient \
		${VPP_FLAGS} --report_level 2 --target hw --platform ${PLATFORM} --link -o divider.xclbin divider.xo ../div_starter.xo --config ../connect-divider.cfg



%/mm.xclbin: %/mm.xo mms.xo mm_connect.cfg
	cd $* && v++ ${VPP_FLAGS} --report_level 2 --target hw --platform ${PLATFORM} --link -o mm.xclbin mm.xo ../mms.xo --config ../mm_connect.cfg

%/mmm.xclbin: %/mmm.xo mms.xo mm_connect.cfg
	cd $* && v++ ${VPP_FLAGS} --report_level 2 --target hw --platform ${PLATFORM} --link -o mmm.xclbin mmm.xo ../mms.xo --config ../mm_connect.cfg


%/moving-average-util.txt: %/moving-average.xclbin moving-average-util.tcl
	cd $* && vivado -mode batch -source ../moving-average-util.tcl -tclargs ./temp/link/vivado/vpl/prj/prj.runs/impl_1/u96v2_sbc_base_wrapper_routed.dcp

%/moving-average-util-filtered.txt: %/moving-average-util.txt util_filter.py
	cd $* && python ../util_filter.py -i moving-average-util.txt -o moving-average-util-filtered.txt

%/mm-util.txt: %/mm.xclbin mm-util.tcl
	cd $* && vivado -mode batch -source ../mm-util.tcl -tclargs ./temp/link/vivado/vpl/prj/prj.runs/impl_1/u96v2_sbc_base_wrapper_routed.dcp

%/mm-util-filtered.txt: %/mm-util.txt util_filter.py
	cd $* && python ../util_filter.py -i mm-util.txt -o mm-util-filtered.txt

%/mmm-util.txt: %/mmm.xclbin mmm-util.tcl
	cd $* && vivado -mode batch -source ../mmm-util.tcl -tclargs ./temp/link/vivado/vpl/prj/prj.runs/impl_1/u96v2_sbc_base_wrapper_routed.dcp

%/mmm-util-filtered.txt: %/mmm-util.txt util_filter.py
	cd $* && python ../util_filter.py -i mmm-util.txt -o mmm-util-filtered.txt




%/divider-util.txt: %/divider.xclbin divider-util.tcl
	cd $* && vivado -mode batch -source ../divider-util.tcl -tclargs ./temp/link/vivado/vpl/prj/prj.runs/impl_1/u96v2_sbc_base_wrapper_routed.dcp

%/divider-util-filtered.txt: %/divider-util.txt util_filter.py
	cd $* && python ../util_filter.py -i divider-util.txt -o divider-util-filtered.txt


mas: $(MA_XCLBINS) $(MAS_XCLBINS) $(MAP_XCLBINS)  

bitstreams: $(MM_XCLBINS) $(MA_XCLBINS) $(MAS_XCLBINS) $(MAP_XCLBINS)  $(MMM_XCLBINS) $(DIV_XCLBINS)

