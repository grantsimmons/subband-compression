#include <vector>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <iterator>
#include <string>

#include "dsp.hh"

struct pixel_grayscale {
    pixel_grayscale(int value) : value(value) {
        
    }

    uint8_t value;
};

int main(int argc, char** argv) {

    std::string file_names[argc-1];
    if(argc == 1) {
        std::cout << "No files given" << std::endl;
        exit(1);
    }
    else if(argc >= 2) {
        for(int counter = 1; counter < argc; counter++)
            file_names[counter-1] = argv[counter];
    }

    std::ifstream infile(file_names[0], std::ios_base::binary);

    std::vector<pixel_grayscale> image_pixel_data( (std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());

    std::vector<double> test = {38,37,28,30,0,0,0,0};

    std::vector<double> test2 = haar1d(test);

    for (auto val : test) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    for (auto val : test2) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    return 0;
}