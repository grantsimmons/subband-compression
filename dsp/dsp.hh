#ifndef SC_DSP_DSP_H_
#define SC_DSP_DSP_H_

#include <cstdint>
#include <vector>
#include <iostream>
#include <functional>

std::vector<double> haar1d(std::vector<double> image);

std::vector<double> ihaar1d(std::vector<double> transform);

void haar2d(std::vector<double>& image, int image_w, int block_h, int block_w);

#endif