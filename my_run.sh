#!/bin/sh

echo Run the simulation.
echo Usage: ./my_run -c test/debug
echo Extra arguments: "$@"


MYPATH=/home/antedo/code_vehicles/results-sim/$(date +"%m%d/%H%M")/
echo $MYPATH

cd src/
opp_makemake -f --deep --make-so -I ../../veins/src -o containerVehicles -O out
cd ..
make
cd simulations/
opp_run -u Cmdenv -m -n .:../src:../../veins/examples/veins:../../veins/src/veins -l ../../veins/src/veins -l ../src/containerVehicles \
        --cmdenv-express-mode=false --cmdenv-log-level=trace  \
        --**.mac.cmdenv-log-level=off \
        --**.nic.*.cmdenv-log-level=off \
        --**.phy.cmdenv-log-level=off \
        --**.appl.cmdenv-log-level=info \
        --**.manager.cmdenv-log-level=off \
        --cmdenv-output-file=log.txt \
        "--scenario.simGod.externalPath=\"${MYPATH}\"" "$@"


