#!/usr/bin/env python
from cv2 import *
import sys

# [1] : input_image
# [2] : output_dir

if __name__=='__main__':
    img = imread(sys.argv[1])
    gray = cvtColor(img,COLOR_RGB2GRAY)
    w, h = gray.shape
    for x in range(w):
        for y in range(h):
            if 0 < gray[x,y] and gray[x,y] < 255:
                gray[x,y] = 1
    #imshow("test",gray)
    #waitKey(0)
    fn_sp = sys.argv[1].split('/')
    fn = sys.argv[2] + fn_sp[len(fn_sp)-1]
    print(fn)
    imwrite(fn,gray)

