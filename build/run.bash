#!/bin/bash

mv output out_bak
mkdir output
cmake ..
make -j
mkdir images wearout labels_tmp
mkdir images_blur wearout_blur labels_gray
./soccerField
./Blur
mv images wearout labels_tmp output
mv images_blur wearout_blur labels_gray output

# mv output ../appendColorMap/
# cd ../appendColorMap
# ./run.bash output/labels_gray
# mv out output/labels
