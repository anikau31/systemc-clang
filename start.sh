#!/bin/bash

# usage: ./start.sh <docker-image> <path-to-sccl-source>
if [ "$#" -ne 4 ]; then
  echo "Usage: ./start.sh <docker-image> <docker-container-name> <path-to-sccl-source> <path-to-sccl-build-directory>"
  exit 0
fi

DOCKER_IMAGE_NAME=$1
DOCKER_CONTAINER_NAME=$2
HOST_SYSTEMC_CLANG=$(readlink -f $3)
HOST_SYSTEMC_CLANG_BUILD_DIR=$(readlink -f $4)


sudo docker run --rm -it --entrypoint bash \
  -v $HOST_SYSTEMC_CLANG:/systemc-clang \
  -v $HOST_SYSTEMC_CLANG_BUILD_DIR:/systemc-clang-build \
  --net=host \
  --name $DOCKER_CONTAINER_NAME \
  $DOCKER_IMAGE_NAME

