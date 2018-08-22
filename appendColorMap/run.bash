#!/bin/bash
rm -r out 
rm -r in
mkdir out
mkdir in
cp $1/* in/
for a in `readlink -f in/*`
do
    python bgr2gray.py $a in/
done
matlab -nodesktop -nosprash -r 'appendColorMap; exit'
