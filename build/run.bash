#!/bin/bash

rm -r images wearout labels
rm -r images_blur wearout_blur labels_gray
cmake ..
make -j
mkdir images wearout labels
mkdir images_blur wearout_blur labels_gray
./soccerField
./Blur
