#ifndef SC_DSP_DSP_H_
#define SC_DSP_DSP_H_

#include <cstdint>
#include <vector>

std::vector<int> ihaar1d(std::vector<int>& image);

std::vector<int> haar1d(std::vector<int>& image);

std::vector<double> haar1d(std::vector<double>& image);

std::vector<double> ihaar1d(std::vector<double>& transform);

void threshold_and_quantize(std::vector<double>& source, const double bin_size, const int image_w, const int block_w, const int block_h, const double threshold);

void dequantize(std::vector<double>& source, const double bin_size, const int image_w, const int block_w, const int block_h);

void threshold_and_quantize2(std::vector<double>& source, const double bin_size, const int image_w, const int block_w, const int block_h, const double threshold);

void dequantize2(std::vector<double>& source, const double bin_size, const int image_w, const int block_w, const int block_h);

#endif