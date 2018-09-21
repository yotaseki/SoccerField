#!/bin/bash

mv output out_bak
mkdir -p output/orig
cmake ..
make -j
mkdir img img_wearout img_gain img_gain_wearout labels_tmp
mkdir original blur wearout wearout_blur gain gain_blur gain_wearout gain_wearout_blur labels_gray
./soccerField
./Blur
mv img img_wearout img_gain img_gain_wearout labels_tmp output/orig
mv original blur wearout wearout_blur gain gain_blur gain_wearout gain_wearout_blur labels_gray output

#mv output ../appendColorMap/
#cd ../appendColorMap
#./run.bash output/labels_gray
#mv out output/labels
