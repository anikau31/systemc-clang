#!/bin/sh

config_=ZCHK,DATAW=64,PREC=64

darg_="1 2 3"
targ=6
rarg_="8 16 24 32 40 48 56 64"
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
