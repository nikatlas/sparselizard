#!/bin/sh

pwd=$(pwd)
echo "Current Working directory: $pwd"


cd simulations/mcvt/models
gmsh model.geo -2


cd $pwd 
./make_script.sh

cd build/simulations/mcvt
./mcvt Parrot
