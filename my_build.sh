#!/bin/sh

cd src/
opp_makemake -f --deep --make-so -I ../../veins/src -o containerVehicles -O out
cd ..
make
