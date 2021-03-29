#ifndef SC_DSP_DSP_H_
#define SC_DSP_DSP_H_

#include <cstdint>
#include <vector>

std::vector<double> haar1d(std::vector<double>& image);

std::vector<double> ihaar1d(std::vector<double>& transform);

std::vector<double> haar2d(const std::vector<uint8_t>& image, const int image_w, const int block_h, const int block_w);

std::vector<uint8_t> ihaar2d(std::vector<double>& transform, const int image_w, const int block_h, const int block_w);

#endif