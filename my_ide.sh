#!/bin/sh

cd src/
opp_makemake -f --deep --make-so -I ../../veins/src -o containerVehicles -O out
cd ..
make
cd simulations/
opp_run -u Qtenv -m -n .:../src:../../veins/examples/veins:../../veins/src/veins -l ../../veins/src/veins -l ../src/containerVehicles --cmdenv-express-mode=false --cmdenv-log-level=trace  --**.mac.cmdenv-log-level=off --**.nic.*.cmdenv-log-level=off --**.phy.cmdenv-log-level=off --**.appl.cmdenv-log-level=trace
