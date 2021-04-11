#include <fstream>

#include "dsp.hh"
#include "dsp_2d_template.hh"
#include "fileio_template.hh"
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
    std::vector<int> image_quant = threshold_and_quantize(image_transform,quantization_bin_size,image_width,wavelet_block_size,wavelet_block_size,quantization_threshold);
    std::vector<double> image_dequant = dequantize(image_quant,quantization_bin_size,image_width,wavelet_block_size,wavelet_block_size);

    //TODO: Entropy Encoding
    //TODO: Huffman Compression

    //TODO: Write to File
    //TODO: Read from File

    //Reconstruct Bitmap from Wavelet Decomposition
    std::vector<uint8_t> reconstruction = ihaar2d<double>(image_dequant,image_width,wavelet_block_size,wavelet_block_size);

    //Display Image
    dump_image(image_pixel_data, "orig.x");
    dump_image(reconstruction, "reconstruction.x");

    std::cout << (image_pixel_data == reconstruction ? "SUCCESS" : "FAILURE") << std::endl;

    std::vector<int> int_transform = haar2d<int>(image_pixel_data,512,8,8);

    auto test = extract_levels_from_serial<int>(image_quant,512,8,8);

    int initial = 64;

    // print_vector(test[0][2],"TEST0D",initial,6);

     auto test_map = generate_frequency_table<int>(image_quant); //Map Wavelet Coefficient Values to the number of their occurrences
    // //auto test_map = generate_frequency_table<int>(test[2][2]);
    // for (auto test : test_map) {
    //     std::cout << (int) test.first << ": " << test.second << std::endl;
    // }

    InternalNode* test1 = generate_huffman_tree<int>(test_map); //Generate Huffman encoding based on frequency information

    std::map<uint32_t,int> serialize_test = serialize_huffman_tree<int>(test1,0); //Serialize the binary tree into a linear map

    canonical_huffman_table<int> canon = generate_canonical_huffman_code<int>(serialize_test); //Generate a canonical Huffman code based on the serialized Huffman code bit lengths

    std::vector<uint32_t> translated = translate_canonical<int,uint32_t>(image_quant, canon.translation_map); //Translate the wavelet coefficients into the new canonical mapping

    std::map<uint32_t,int> inverse = invert_map<int,uint32_t>(canon.translation_map);

    std::vector<int> recovered = translate_canonical<uint32_t,int>(translated,inverse); //Generate a fake map reconstruction

    std::vector<int> canonical_loss = diff<int,int>(image_quant,recovered);

    for(auto val : serialize_test) {
        std::cout << val.first << " " << val.second << std::endl;
    }

    image_header header(512,512,8,canon.max_bits,true,0);

    io_write_buf out_buf("test.bin");
    out_buf.write_header(header);
    out_buf.write_canonical_huffman_table<int>(canon);
    out_buf.write_data<uint32_t>(translated);
    header.print();
    out_buf.close();

    io_read_buf<int> in_buf("test.bin");
    image_header test_header = in_buf.read_header();
    canonical_huffman_table<int> recovered_table = in_buf.read_canonical_huffman_table();
    std::cout << (recovered_table.canonical_table == canon.canonical_table ? "OH YES" : "OH NO") << std::endl;
    for(int i = 0; i < canon.canonical_table.size(); i++) {
        std::cout << "Canon: " << canon.canonical_table[i].first << ", " << canon.canonical_table[i].second << " ";
        std::cout << "Recovered: " << recovered_table.canonical_table[i].first << ", " << recovered_table.canonical_table[i].second << std::endl;
    }
    std::vector<int> hail_mary = in_buf.read_data();
    std::cout << "Vector size: " << hail_mary.size() << std::endl;
    std::cout << "Original size: " << translated.size() << std::endl;
    print_vector<std::vector<int>>(diff<int,int>(image_quant, hail_mary), "ACTUAL RECONSTRUCTION", 512);
    std::vector<double> will_this_work_d = dequantize(hail_mary,quantization_bin_size,image_width,wavelet_block_size,wavelet_block_size);
    std::vector<uint8_t> yes_it_will = ihaar2d<double>(will_this_work_d,512,8,8);
    dump_image(yes_it_will, "wtf.x");
    test_header.print();


    

    // for(int i = 1; i < 4; i++) {
    //     for(int j = 0; j < 3; j++) {
    //         auto test_map = generate_frequency_table<double>(test[i][j]);
    //         std::cout << i << "," << j << std::endl;
    //         print_vector(test[i][j],"TEST",initial);
    //         for (auto test : test_map) {
    //             std::cout << (int) test.first << ": " << test.second << std::endl;
    //         }
    //     }
    // }
    // print_vector(test[1][0],"TEST1H",initial,6);
    // print_vector(test[1][1],"TEST1V",initial,6);
    // print_vector(test[1][2],"TEST1D",initial,6);
    // print_vector(test[2][0],"TEST2H",initial*2,6);
    // print_vector(test[2][1],"TEST2V",initial*2,6);
    // print_vector(test[2][2],"TEST2D",initial*2,6);
    // print_vector(test[3][0],"TEST3H",initial*4,6);
    // print_vector(test[3][1],"TEST3V",initial*4,6);
    // print_vector(test[3][2],"TEST3D",initial*4,6);

    return 0;
}