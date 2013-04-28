#!/bin/bash

rm galib247.tgz
wget http://lancet.mit.edu/ga/dist/galib247.tgz
tar -xzf galib247.tgz
cd galib247
make
cd ..

