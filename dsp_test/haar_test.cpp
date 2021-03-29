#include <fstream>

#include "dsp.hh"
#include "dsp_2d_template.hh"
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
    std::vector<double> image_transform = haar2d<double>(image_pixel_data,512,8,8);

    //TODO: Scalar Quantize for Lossy Compression
    quantize(image_transform,0.1,8,8,8);

    print_vector(image_transform, "QUANTIZED TRANSFORM", 512);
    //TODO: Entropy Encoding
    //TODO: Huffman Compression

    //TODO: Write to File
    //TODO: Read from File

    //Reconstruct Bitmap from Wavelet Decomposition
    std::vector<uint8_t> reconstruction = ihaar2d<double>(image_transform,512,8,8);
    std::vector<double> test(reconstruction.begin(), reconstruction.end());

    //Display Image

    //print_vector(reconstruction, "RECONSTRUCTED IMAGE", 512);

    if(image_pixel_data == reconstruction) {
        std::cout << "SUCCESS" << std::endl;
    }

    std::vector<int> int_transform = haar2d<int>(image_pixel_data,512,8,8);

    std::vector<uint8_t> int_reconstruction = ihaar2d<int>(int_transform,512,8,8);

    if(image_pixel_data == int_reconstruction) {
        std::cout << "SUCCESS" << std::endl;
    }

    return 0;
}