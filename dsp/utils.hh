#ifndef SC_DSP_UTILS_H
#define SC_DSP_UTILS_H

#include <cstdint>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <bits/stdc++.h>

#define clz(x) __builtin_clz(x)

template <class VectorIterator>
std::vector<typename std::iterator_traits<VectorIterator>::value_type> extract_every_nth(VectorIterator element, const size_t n, const size_t max) {
    std::vector<typename std::iterator_traits<VectorIterator>::value_type> ret;
    //Quick, inneficient hack to get an image block column
    //Yeah, this isn't great but it's easier to get the type from the iterator than to let the template know that the type has an iterator

    for (int i = 0; i < max; i++) {
        ret.push_back(*(element));
        std::advance(element,n);
    }

    return ret;
}

template <typename T>
std::map<int,std::map<int,std::vector<T>>> extract_levels_from_serial(std::vector<T>& serial, const int image_w, const int block_h, const int block_w) {
    int levels = 0;
    if(block_h == block_w &&  (!(block_h & (block_h - 1)) && block_h)) { // Check that block_h and block_w are equal and power of 2
        levels = sizeof(decltype(block_h)) * CHAR_BIT - clz(block_h) - 1; // Get log 2 of power of 2. Thanks https://stackoverflow.com/questions/47074126/log2-of-an-integer-that-is-a-power-of-2
    }

    std::map<int,std::map<int,std::vector<T>>> ret;
    enum subband {HORIZONTAL,VERTICAL,DIAGONAL} direction;

    for(size_t i = 0; i < serial.size(); i++) {
        int block_y_index = (i / image_w) % block_h;
        int block_x_index = i % block_w;

        int block_x_level = block_x_index == 0 ? 0 : sizeof(decltype(block_x_index)) * CHAR_BIT - clz(block_x_index);
        int block_y_level = block_y_index == 0 ? 0 : sizeof(decltype(block_y_index)) * CHAR_BIT - clz(block_y_index);

        int index_level;
        if(block_x_level > block_y_level) {
            //std::cout << "Pushing index" << i << " (" << block_x_index << "," << block_y_index <<"), value " << serial[i] << " to " << block_x_level << "h" << std::endl;
            //std::cout << "Pushing to " << block_x_level << "h" << std::endl;
            ret[block_x_level][HORIZONTAL].push_back(serial[i]);
        }
        else if(block_y_level > block_x_level) {
            //std::cout << "Pushing index" << i << " (" << block_x_index << "," << block_y_index <<"), value " << serial[i] << " to " << block_y_level << "v" << std::endl;
            //std::cout << "Pushing to " << block_y_level << "v" << std::endl;
            ret[block_y_level][VERTICAL].push_back(serial[i]);
        }
        else if(block_x_level == block_y_level) {
            //std::cout << "Pushing index" << i << " (" << block_x_index << "," << block_y_index <<"), value " << serial[i] << " to " << block_x_level << "d" << std::endl;
            //std::cout << "Pushing to " << block_x_level << "d" << std::endl;
            ret[block_x_level][DIAGONAL].push_back(serial[i]);
        }
        else {
            std::cout << "WHOOPS" << std::endl;
        }
    }

    return ret;
}

template <class VectorType>
int print_vector(VectorType in, const std::string desc, const int width = 0, const int val_width = 4) {
    std::cout << desc << std::endl;
    int i = 0;
    int non_zero = 0;

    for (int i = 0; i < in.size(); i++) {
        if (in[i] != 0) non_zero++;
        std::cout << std::setw(val_width) << (double) in[i] << " ";
        if (width != 0) {
            if (i % width == width - 1)
                std::cout << "\n";
        }
    }

    std::cout << "\n";

    return non_zero;
}

template <class InIterator, class OutIterator>
void copy_every_nth(InIterator b, InIterator e, OutIterator r, const size_t n, const size_t max) {
    for (size_t i = 0; i < max; i++) {
        *r = *b++;
        std::advance(r,n);
    }
}

template <typename T> 
int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

template <typename T, typename U>
std::vector<T> diff(std::vector<U> one, std::vector<U> two) {
    std::vector<T> diff_vector;

    for(size_t i = 0; i < one.size(); i++) {
        diff_vector.push_back(abs(one[i] - two[i]));
    }

    return diff_vector;
}

static void dump_image(std::vector<uint8_t>& image, std::string filename = "img.x") {
    std::ofstream bin(filename, std::ios::out | std::ios::binary);
    bin.write((char*) &image[0], image.size());
    bin.close();
}

static void dump_image(std::vector<double>& d_image, std::string filename = "img.x") {
    std::vector<uint8_t> image(d_image.begin(), d_image.end());
    std::ofstream bin(filename, std::ios::out | std::ios::binary);
    bin.write((char*) &image[0], image.size());
    bin.close();
}

#endif
