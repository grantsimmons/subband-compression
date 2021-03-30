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
    std::vector<uint8_t> image_pixel_data( (std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());

    //Parameters:
    const double quantization_bin_size = 0.25; //0.25
    const int wavelet_block_size = 16; //8
    const int image_width = 512;
    const double quantization_threshold = 0.8; //0.8

    //Transform Image into Wavelet Decomposition
    std::vector<double> image_transform = haar2d<double>(image_pixel_data,image_width,wavelet_block_size,wavelet_block_size);

    //Threshold and Scalar Quantize for Lossy Compression
    threshold_and_quantize(image_transform,quantization_bin_size,image_width,wavelet_block_size,wavelet_block_size,quantization_threshold);
    dequantize(image_transform,quantization_bin_size,image_width,wavelet_block_size,wavelet_block_size);

    //TODO: Entropy Encoding
    //TODO: Huffman Compression

    //TODO: Write to File
    //TODO: Read from File

    //Reconstruct Bitmap from Wavelet Decomposition
    std::vector<uint8_t> reconstruction = ihaar2d<double>(image_transform,image_width,wavelet_block_size,wavelet_block_size);
    std::vector<double> test(reconstruction.begin(), reconstruction.end());

    //Display Image
    dump_image(image_pixel_data, "orig.x");
    dump_image(reconstruction, "reconstruction.x");

    //print_vector(reconstruction, "RECONSTRUCTED IMAGE", 512);

    std::cout << (image_pixel_data == reconstruction ? "SUCCESS" : "FAILURE") << std::endl;

    std::vector<int> int_transform = haar2d<int>(image_pixel_data,512,8,8);

    std::vector<uint8_t> int_reconstruction = ihaar2d<int>(int_transform,512,8,8);

    if(image_pixel_data == int_reconstruction) {
        std::cout << "SUCCESS" << std::endl;
    }

/*    
    std::vector<uint8_t> v =    {38,30,28,30,98,29,10,111,39,30,28,30,98,29,10,111,
                                 38,31,28,31,98,29,11,112,39,31,28,31,98,29,11,112,
                                 38,32,28,32,98,29,12,113,39,32,28,32,98,29,12,113,
                                 38,33,28,33,98,29,13,114,39,33,28,33,98,29,13,114,
                                 38,34,28,34,98,29,14,115,39,34,28,34,98,29,14,115,
                                 38,35,28,35,98,29,15,116,39,35,28,35,98,29,15,116,
                                 38,36,28,36,98,29,16,117,39,36,28,36,98,29,16,117,
                                 38,37,28,37,98,29,17,118,39,37,28,37,98,29,17,118,
                                 37,30,28,30,98,29,10,111,36,30,28,30,98,29,10,111,
                                 37,31,28,31,98,29,11,112,36,31,28,31,98,29,11,112,
                                 37,32,28,32,98,29,12,113,36,32,28,32,98,29,12,113,
                                 37,33,28,33,98,29,13,114,36,33,28,33,98,29,13,114,
                                 37,34,28,34,98,29,14,115,36,34,28,34,98,29,14,115,
                                 37,35,28,35,98,29,15,116,36,35,28,35,98,29,15,116,
                                 37,36,28,36,98,29,16,117,36,36,28,36,98,29,16,117,
                                 37,37,28,37,98,29,17,118,36,37,28,37,98,29,17,118
                                 };

    print_vector(v, "ORIGINAL", 16);

    std::vector<double> v_transform = haar2d<double>(v,16,8,8);

    print_vector(v_transform, "TRANSFROMED", 16);

    const double threshold = 0.2;

    threshold_and_quantize(v_transform,0.2,16,8,8,0.26);

    print_vector(v_transform, "QUANTIZED", 16);

    dequantize(v_transform,0.2,16,8,8);
    
    print_vector(v_transform, "DEQUANTIZED", 16);

    std::vector<uint8_t> v_reconstructed = ihaar2d<double>(v_transform,16,8,8);

    print_vector(v_reconstructed, "RECONSTRUCTED", 16);
*/
    return 0;
}