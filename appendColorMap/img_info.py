#!/usr/bin/env python
import cv2
import sys

if __name__ == "__main__":
    img = cv2.imread(sys.argv[1], cv2.IMREAD_UNCHANGED)

    if img is None:
        print("Failed to load image file.")
        sys.exit(1)

    if len(img.shape) == 3:
        height, width, channels = img.shape[:3]
    else:
        height, width = img.shape[:2]
        channels = 1

    print("width: " + str(width))
    print("height: " + str(height))
    print("channels: " + str(channels))
    print("dtype: " + str(img.dtype))
