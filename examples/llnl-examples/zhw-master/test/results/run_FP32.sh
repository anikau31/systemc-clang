#!/bin/sh

config_=ZCHK,DATAW=32,PREC=32

darg_="1 2 3"
targ=6
rarg_="4 8 12 16 20 24 28 32"
aarg_="1 2 3 4 5 6 7 8"

for darg in $darg_ ; do
# make application
make -B D=$config_,DIMS=$darg

for rarg in $rarg_ ; do
for aarg in $aarg_ ; do
  # barg=aarg^darg;
  barg=1; for ((i=0;i<darg;i++)); do barg=$((barg*aarg)); done

  args="-t$targ -r$rarg -b$barg"
  # echo $args

  # run application
  ./test $args

done
done
done
