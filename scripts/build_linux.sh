#!/bin/bash
set -e

if [ "$#" -ne 1 ]
then
  echo "Usage: $0 <linux-distro-name>"
  exit 1
fi

cd ..
mkdir -p build/linux_"$1"/debug
cd build/linux_"$1"/debug
cmake -DCMAKE_BUILD_TYPE=Debug -DINFERNAL_LINUX_DISTRO="$1" ../../..
make

cd ..
mkdir -p release
cd release
cmake -DCMAKE_BUILD_TYPE=Release -DINFERNAL_LINUX_DISTRO="$1" ../../..
make

cd ../../../scripts
