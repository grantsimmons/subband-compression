#include <vector>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <functional>

#include "dsp.hh"
#include "utils.hh"

int main(int argc, char** argv) {

    //Check command line arguments. As of now, take a single image file
    std::string file_names[argc-1];
    if(argc == 1) {
        std::cout << "No files given" << std::endl;
        exit(1);
    }
    else if(argc >= 2) {
        for(int counter = 1; counter < argc; counter++)
            file_names[counter-1] = argv[counter];
    }

    //Open file
    std::ifstream infile(file_names[0], std::ios_base::binary);

    //Read raw image data
    //TODO: Implement OpenImageIO to read other data formats to internal structure
    const std::vector<uint8_t> image_pixel_data( (std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());

    //Transform Image into Wavelet Decomposition
    std::vector<double> image_transform = haar2d(image_pixel_data,512,8,8);

    //Compress

    return 0;
}