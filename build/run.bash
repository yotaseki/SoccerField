#!/bin/bash

mv output out_bak
mkdir output
cmake ..
make -j
mkdir images wearout labels
mkdir images_blur wearout_blur labels_gray
./soccerField
./Blur
mv images wearout labels output
mv images_blur wearout_blur labels_gray output
