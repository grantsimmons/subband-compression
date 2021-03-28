#include <vector>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <functional>

#include "dsp.hh"
#include "utils.hh"

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

    std::vector<uint8_t> image_pixel_data( (std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());

    std::vector<double> image_pixel_data_d(image_pixel_data.begin(), image_pixel_data.end());

    //ihaar1d(test2);

    std::vector<double> v =    {38,30,28,30,98,29,10,111,
                                38,31,28,31,98,29,11,112,
                                38,32,28,32,98,29,12,113,
                                38,33,28,33,98,29,13,114,
                                38,34,28,34,98,29,14,115,
                                38,35,28,35,98,29,15,116,
                                38,36,28,36,98,29,16,117,
                                38,37,28,37,98,29,17,118};


    //haar2d(v,8,8,8);

    haar2d(image_pixel_data_d,512,4,4);

    return 0;
}