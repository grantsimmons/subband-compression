IMG ?= img
IMG_X ?= 512
IMG_Y ?= 512

default:
	g++ --std=c++17 -g -Wall dsp_test/haar_test.cpp dsp/dsp.cc -o haar_test.out -I dsp

.PHONY: img

img:
	convert -depth 8 -size $(IMG_X)x$(IMG_Y)+0 gray:$(IMG).x $(IMG).png