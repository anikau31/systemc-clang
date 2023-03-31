#!/bin/bash
rsync -av --include="*/" --include="*.xclbin"  --exclude="*" --prune-empty-dirs  . $ULTRA96_V2_SSH_NAME:~/ae/
rsync -aP "../../hardware/app/MCDMA.py" ${ULTRA96_V2_SSH_NAME}:~/jupyter_notebooks/
rsync -aP "../../hardware/app/utils.py" ${ULTRA96_V2_SSH_NAME}:~/jupyter_notebooks/
rsync -arP "../../hardware/app/data" ${ULTRA96_V2_SSH_NAME}:~/ae/
rsync -arP "../../../systemc-clang-build-zfp/hardware/rtl/DS_top_z3_d64/DS_top_z3_d64.bit" ${ULTRA96_V2_SSH_NAME}:~/ae/DS_top_z3_d64.bit
rsync -arP "../../../systemc-clang-build-zfp/hardware/rtl/DS_top_z3_d32/DS_top_z3_d32.bit" ${ULTRA96_V2_SSH_NAME}:~/ae/DS_top_z3_d32.bit
rsync -arP "../../../systemc-clang-build-zfp/hardware/rtl/DS_top_z7_d64/DS_top_z7_d64.bit" ${ULTRA96_V2_SSH_NAME}:~/ae/DS_top_z7_d64.bit
rsync -arP "../../../systemc-clang-build-zfp/hardware/rtl/DS_top_z7_d32/DS_top_z7_d32.bit" ${ULTRA96_V2_SSH_NAME}:~/ae/DS_top_z7_d32.bit
# Quick hack for handling hwh files
rsync -arP "../../../systemc-clang-build-zfp/hardware/bd/bd/.gen/sources_1/bd/bd/hw_handoff/bd.hwh" ${ULTRA96_V2_SSH_NAME}:~/ae/DS_top_z3_d64.hwh
rsync -arP "../../../systemc-clang-build-zfp/hardware/bd/bd/.gen/sources_1/bd/bd/hw_handoff/bd.hwh" ${ULTRA96_V2_SSH_NAME}:~/ae/DS_top_z3_d32.hwh
rsync -arP "../../../systemc-clang-build-zfp/hardware/bd/bd/.gen/sources_1/bd/bd/hw_handoff/bd.hwh" ${ULTRA96_V2_SSH_NAME}:~/ae/DS_top_z7_d64.hwh
rsync -arP "../../../systemc-clang-build-zfp/hardware/bd/bd/.gen/sources_1/bd/bd/hw_handoff/bd.hwh" ${ULTRA96_V2_SSH_NAME}:~/ae/DS_top_z7_d32.hwh

