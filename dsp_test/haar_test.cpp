#include <fstream>

#include "dsp.hh"
#include "dsp_2d_template.hh"
#include "huffman_template.hh"
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
    const int wavelet_block_size = 8; //8
    const int image_width = 512;
    const double quantization_threshold = 0.8; //0.8

    //Transform Image into Wavelet Decomposition
    std::vector<double> image_transform = haar2d<double>(image_pixel_data,image_width,wavelet_block_size,wavelet_block_size);
    std::vector<double> image_transform2 = image_transform;

    //Threshold and Scalar Quantize for Lossy Compression
    threshold_and_quantize(image_transform,quantization_bin_size,image_width,wavelet_block_size,wavelet_block_size,quantization_threshold);
    //dequantize(image_transform,quantization_bin_size,image_width,wavelet_block_size,wavelet_block_size);

    threshold_and_quantize2(image_transform2,quantization_bin_size,image_width,wavelet_block_size,wavelet_block_size,quantization_threshold);
    //dequantize2(image_transform2,quantization_bin_size,image_width,wavelet_block_size,wavelet_block_size);

    //TODO: Entropy Encoding
    //TODO: Huffman Compression

    //TODO: Write to File
    //TODO: Read from File

    //Reconstruct Bitmap from Wavelet Decomposition
    std::vector<uint8_t> reconstruction = ihaar2d<double>(image_transform,image_width,wavelet_block_size,wavelet_block_size);
    std::vector<uint8_t> reconstruction2 = ihaar2d<double>(image_transform2,image_width,wavelet_block_size,wavelet_block_size);
    //std::vector<double> test(reconstruction.begin(), reconstruction.end());

    std::vector<int> diff_vec = diff<int>(reconstruction, reconstruction2);
    std::vector<uint8_t> u_diff_vec(diff_vec.begin(), diff_vec.end());

    //Display Image
    dump_image(image_pixel_data, "orig.x");
    dump_image(reconstruction, "reconstruction.x");
    dump_image(reconstruction2, "reconstruction_no.x");
    dump_image(u_diff_vec, "diff.x");

    int non_zero = print_vector(diff_vec, "DIFF", 512);
    std::cout << "NON_ZERO: " << non_zero << std::endl;

    //print_vector(reconstruction, "RECONSTRUCTED IMAGE", 512);

    std::cout << (image_pixel_data == reconstruction ? "SUCCESS" : "FAILURE") << std::endl;

    std::vector<int> int_transform = haar2d<int>(image_pixel_data,512,8,8);
/*
    std::vector<uint8_t> int_reconstruction = ihaar2d<int>(int_transform,512,8,8);

    if(image_pixel_data == int_reconstruction) {
        std::cout << (image_pixel_data == int_reconstruction ? "SUCCESS" : "FAILURE") << std::endl;
    }
 */
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

    auto v_transform = haar2d<double>(v,16,8,8);

    auto test = extract_levels_from_serial(image_transform,512,8,8);

    print_vector(v_transform,"TRANSFORM",16,6);

    int initial = 64;

    print_vector(test[0][2],"TEST0D",initial,6);
    auto test_map = generate_frequency_table<double>(test[0][2]);
    for (auto test : test_map) {
        std::cout << (int) test.first << ": " << test.second << std::endl;
    }
    for(int i = 1; i < 4; i++) {
        for(int j = 0; j < 3; j++) {
            auto test_map = generate_frequency_table<double>(test[i][j]);
            std::cout << i << "," << j << std::endl;
            //print_vector(test[i][j],"TEST",initial,6);
            for (auto test : test_map) {
                std::cout << (int) test.first << ": " << test.second << std::endl;
            }
        }
    }
    // print_vector(test[1][0],"TEST1H",initial,6);
    // print_vector(test[1][1],"TEST1V",initial,6);
    // print_vector(test[1][2],"TEST1D",initial,6);
    // print_vector(test[2][0],"TEST2H",initial*2,6);
    // print_vector(test[2][1],"TEST2V",initial*2,6);
    // print_vector(test[2][2],"TEST2D",initial*2,6);
    // print_vector(test[3][0],"TEST3H",initial*4,6);
    // print_vector(test[3][1],"TEST3V",initial*4,6);
    // print_vector(test[3][2],"TEST3D",initial*4,6);

/*
    print_vector(image_pixel_data, "IMAGE", 512);
    auto test_map = generate_frequency_table<uint8_t>(image_pixel_data);
    for (auto test : test_map) {
        std::cout << (int) test.first << ": " << test.second << std::endl;
    }
    std::cout << "SIZE: " << test_map.size() << std::endl;
    std::cout << std::endl;

    print_vector(image_transform, "TRANSFORM", 512);

    auto test_map1 = generate_frequency_table<double>(image_transform);
    for (auto test : test_map1) {
        std::cout << (double) test.first << ": " << test.second << std::endl;
    }
    std::cout << "SIZE: " << test_map1.size() << std::endl;
    std::cout << std::endl;

    print_vector(int_transform, "INT TRANSFORM", 512);

    auto test_map2 = generate_frequency_table<int>(int_transform);
    for (auto test : test_map2) {
        std::cout << (int) test.first << ": " << test.second << std::endl;
    }
    std::cout << "SIZE: " << test_map2.size() << std::endl;
    std::cout << std::endl;
/*
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