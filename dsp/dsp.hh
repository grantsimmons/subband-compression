#ifndef SC_DSP_DSP_H_
#define SC_DSP_DSP_H_

#include <cstdint>
#include <vector>

std::vector<int> ihaar1d(std::vector<int>& image);

std::vector<int> haar1d(std::vector<int>& image);

std::vector<double> haar1d(std::vector<double>& image);

std::vector<double> ihaar1d(std::vector<double>& transform);

std::vector<int> threshold_and_quantize(std::vector<double>& source, const int bins_per_step, const bool invert_bins_per_step, const int image_w, const int block_w, const int block_h, const double threshold);

std::vector<double> dequantize(std::vector<int>& source, const int bins_per_step, const bool invert_bins_per_step, const int image_w, const int block_w, const int block_h);

#endif