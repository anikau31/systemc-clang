#!/bin/bash

git clone https://github.com/anikau31/systemc-clang.git

cd systemc-clang;

DIR=`pwd`

# Pull the image
docker pull rseac/systemc-clang:clang-13.0.0

echo "Building in $DIR"

# Start run the image
output=$( docker ps -a -f name=systemc-clang| grep systemc-clang 2> /dev/null )
if [[ ! -z ${output} ]]; then 
  echo "A container with a name: systemc-clang exists and has status: $( echo ${output} | awk '{ print $7 }' )"
  docker rm systemc-clang
  docker stop systemc-clang
  #docker start systemc-clang
else
  echo "Container with a name: systemc-clang does not exist"
  #docker start systemc-clang
fi

# start the build

docker run -itd --entrypoint /bin/bash -v $DIR:/systemc-clang --name systemc-clang  rseac/systemc-clang:clang-13.0.0
docker exec systemc-clang bash -c "cd /systemc-clang-build; cmake /systemc-clang -DHDL=ON -DENABLE_TESTS=ON -DENABLE_VERILOG_TESTS=ON -G Ninja && ninja && ctest"
