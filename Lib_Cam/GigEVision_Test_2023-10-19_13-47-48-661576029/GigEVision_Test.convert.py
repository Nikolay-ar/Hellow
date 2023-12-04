#!/usr/bin/env python3

# https://stackoverflow.com/questions/58688633/how-to-convert-bayerrg8-format-image-to-rgb-image

import numpy as np
from skimage.io import imsave

# Width and height of Bayer image, not original which is w/2 x h/2      ������ � ������ ����������� Bayer, �� �������������, ������� ���������� w/ 2 x h/ 2
w, h = 2064, 1544
ow, oh = w//2, h//2

# Load in Bayer8 image, assumed raw 8-bit RGGB                          ������ � ������ ����������� Bayer, �� �������������, ������� ���������� w/ 2 x h/ 2
bayer = np.fromfile('./Data/GigEVision_Test.rg8', dtype=np.uint8).reshape((h,w))

# Pick up raw uint8 samples                                             �������� �������������� ������� uint8
R  = bayer[0::2, 0::2]     # rows 0,2,4,6 columns 0,2,4,6
G0 = bayer[0::2, 1::2]     # rows 0,2,4,6 columns 1,3,5,7
G1 = bayer[1::2, 0::2]     # rows 1,3,5,7 columns 0,2,4,6
B  = bayer[1::2, 1::2]     # rows 1,3,5,7 columns 1,3,5,7

# Chop any left-over edges and average the 2 Green values               �������� ��� ���������� ���� � ��������� 2 ������� ��������
R = R[:oh,:ow]
G = G0[:oh,:ow]//2 + G1[:oh,:ow]//2
B = B[:oh,:ow]

# Formulate image by stacking R, G and B and save                       ������������� �����������, ������ R, G � B, � ���������
out = np.dstack((R,G,B))
imsave('./Data/GigEVision_Test.interpolated-py.bmp',out)
