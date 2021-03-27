#ifndef SC_DSP_DSP_H_
#define SC_DSP_DSP_H_

#include <cstdint>
#include <vector>
#include <iostream>

struct pixel_grayscale;

std::vector<double> haar1d(std::vector<double> image);

std::vector<double> ihaar1d(std::vector<double> transform);

#endif