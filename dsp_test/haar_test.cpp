#include <fstream>

#include "dsp.hh"
#include "dsp_2d_template.hh"
#include "fileio_template.hh"
#include "huffman_template.hh"
#include "utils.hh"

int main(int argc, char** argv) {
        std::string infile_name;
        bool reading = true;
        bool debug = false;

        if(argc == 1) {
            std::cout << "No files given" << std::endl;
            exit(1);
        }
        else if(argc == 3) {
            if((std::string) argv[1] == "-r") {
                reading = true;
            }
            else if((std::string) argv[1] == "-w") {
                reading = false;
            }
            else if((std::string) argv[1] == "-d") {
                debug = true;
            }
            else {
                //help()
            }
            infile_name = argv[2];
        }
        else {
            //help()
            exit(1);
        }

    if (!reading) {

        //Open provided raw image data file
        std::ifstream infile(infile_name, std::ios_base::binary);

        //Read raw image data
        //TODO: Implement OpenImageIO to read other data formats to internal structure
        std::vector<uint8_t> image_pixel_data( (std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());

        //Compression Parameters:
        const bool quantization_invert_bin_step = false;
            //Added to compactly represent fractional numbers (for heavy compression)
            //When quantization_invert_bin_step is false, quantization_bin_step represents the number of steps in a single digit increment
                //E.g. quantization_invert_bin_step = false, quantization_bin_step = 4 => 4 bins between each floating point digit
            //When quantization_invert_bin_step is true, quantization_bin_step represents the number of digit increments in a single quantization step
                //E.g. quantization_invert_bin_step = true, quantization_bin_step = 4 => 0.25 bins between each floating point digit (or 4 digits per quantization step)
                //Only use quantization_invert_bin_step when heavy lossy compression is desired
        
        const double quantization_bin_step = 4; //Best results: 4
            //Magnitude of quantization
            //If quantization_invert_bin_step is true, a higher value implies fewer quantization bins (more information loss)
        
        const int wavelet_block_size = 8; //Best results: 8
            //Size of an image block
            //Determine the size of an independent pixel block to be transformed 
        
        const int image_width = 1000;
        const int image_height = 1000;
        
        const double quantization_threshold = 0.8; //Best results: 0.8
            //All transform coefficients whose absolute values are below this value are set to 0
            //TODO: Any value below 0.8 breaks output image

        //Transform Image into Wavelet Decomposition
        std::vector<double> image_transform = haar2d<double>(image_pixel_data,image_width,wavelet_block_size,wavelet_block_size);

        //Threshold and Scalar Quantize for Lossy Compression
        std::vector<int> image_quant = threshold_and_quantize(image_transform, quantization_bin_step,quantization_invert_bin_step, image_width,wavelet_block_size,wavelet_block_size,quantization_threshold);
        std::vector<double> image_dequant = dequantize(image_quant, quantization_bin_step, quantization_invert_bin_step, image_width,wavelet_block_size,wavelet_block_size);

        //Reconstruct Bitmap from Wavelet Decomposition
        std::vector<uint8_t> reconstruction = ihaar2d<double>(image_dequant,image_width,wavelet_block_size,wavelet_block_size);

        //Display Image Without File I/O
        if(debug) {
            dump_image(image_pixel_data, "orig.x");
            dump_image(reconstruction, "reconstruction.x");
        }

        //Huffman Coding
        auto symbol_frequency_table = generate_frequency_table<int>(image_quant); //Map Wavelet Coefficient Values to the number of their occurrences
        InternalNode* huffman_root = generate_huffman_tree<int>(symbol_frequency_table); //Generate Huffman encoding based on frequency information
        
        //Canonical Coding
        std::map<uint32_t,int> huffman_serialized = serialize_huffman_tree<int>(huffman_root,0); //Serialize the binary tree into a linear map
        canonical_huffman_table<int> canon = generate_canonical_huffman_code<int>(huffman_serialized); //Generate a canonical Huffman code based on the serialized Huffman code bit lengths

        //Data Translation
        std::vector<uint32_t> data_translated = translate_canonical<int,uint32_t>(image_quant, canon.translation_map); //Translate the wavelet coefficients into the new canonical mapping

        //Compile metadata into header
        image_header header(image_width, image_height, wavelet_block_size, canon.max_bits, false, 0, quantization_invert_bin_step, quantization_bin_step);

        //File Write
        io_write_buf out_buf("out.wlt");

        out_buf.write_header(header);
        out_buf.write_canonical_huffman_table<int>(canon);
        out_buf.write_data<uint32_t>(data_translated);
        //header.print();

        out_buf.close();

    //End Compression Flow
    }



    if (reading) {
    //Begin Decompression Flow
        
        //File Read

        io_read_buf<int> in_buf(infile_name); //Open image file with buffer

        image_header recovered_header = in_buf.read_header(); //Read Header
        in_buf.read_canonical_huffman_table(); //Read Huffman Table 
            //TODO: Remove redundant vector return
        std::vector<int> recovered_data = in_buf.read_data();

        //Dequantize decoded data
        std::vector<double> recovered_coeffs = dequantize(recovered_data, recovered_header.quantization_step, recovered_header.flag_data.invert_quantization_step, recovered_header.x, recovered_header.block_size, recovered_header.block_size);

        //Inverse Transform image data
        std::vector<uint8_t> recovered_pixels = ihaar2d<double>(recovered_coeffs, recovered_header.x, recovered_header.block_size, recovered_header.block_size);
        dump_image(recovered_pixels, "recovered.x"); //Dump to file for demonstration purposes

    //End Decompression Flow
    }

    return 0;
}


/*
    if(debug) {
        //std::cout << (recovered_table.canonical_table == canon.canonical_table ? "OH YES" : "OH NO") << std::endl; 
        
        recovered_header.print();

        //for(int i = 0; i < canon.canonical_table.size(); i++) {
        //    std::cout << "Canon: " << canon.canonical_table[i].first << ", " << canon.canonical_table[i].second << " ";
        //    std::cout << "Recovered: " << recovered_table.canonical_table[i].first << ", " << recovered_table.canonical_table[i].second << std::endl;
        //}

        std::cout << "Recovered Data size: " << recovered_data.size() << std::endl;
        //std::cout << "Original size: " << data_translated.size() << std::endl;    
        //print_vector<std::vector<int>>(diff<int,int>(image_quant, recovered_data), "ACTUAL RECONSTRUCTION DIFF", recovered_header.x);
    }
*/
    
    //std::vector<int> int_transform = haar2d<int>(image_pixel_data,512,8,8); //Lossless Compression

    //auto test = extract_levels_from_serial<int>(image_quant,512,8,8); //Experimental

    // //auto test_map = generate_frequency_table<int>(test[2][2]);
    // for (auto test : test_map) {
    //     std::cout << (int) test.first << ": " << test.second << std::endl;
    // }



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