#!/bin/bash
set -e

if [ "$#" -ne 1 ]
then
  echo "Usage: $0 <path-to-hiir>"
  exit 1
fi

echo "Setting up vst sdk..."
./setup_vst3sdk_linux.sh

echo "Building infernal..."
./build_infernal_linux.sh $1
