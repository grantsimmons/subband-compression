#include "dsp.hh"

#include "utils.hh"

#include <iostream>

std::vector<double> haar1d(std::vector<double>& image) {
    std::vector<double> image_transform = image;
    //Manual copy for clarity that function takes image and returns transform

    const int size_original = image_transform.size();

    if (size_original <= 1) {
        return image_transform;
    }

    const int size_temp = size_original / 2;

    std::vector<double> coarse_temp;
    std::vector<double> fine_temp;

    for (int i = 0; i < size_original; i += 2) {
        double coarse_avg = (image_transform[i] + image_transform[i+1]) / 2;
        coarse_temp.push_back(coarse_avg);

        double fine_delta = image_transform[i] - coarse_avg;
        fine_temp.push_back(fine_delta);
    }

    std::copy(fine_temp.begin(), fine_temp.end(), std::next(image_transform.begin(), size_temp));
    
    coarse_temp = haar1d(coarse_temp);

    std::copy(coarse_temp.begin(), coarse_temp.end(), image_transform.begin());

    return image_transform;
}

std::vector<double> ihaar1d(std::vector<double>& image_transform) {
    std::vector<double> image = image_transform;
    //Manual copy for clarity that function takes transform and returns image

    int coefficient_count = 1;
    int size_original = image.size();

    std::vector<double> temp;

    //Unpack Coefficients
    while(coefficient_count < size_original) {
        //Iterate Coefficients
        temp.resize(coefficient_count * 2);

        int j = 0; //Temp index

        for(int i = 0; i < coefficient_count; i++) {
            temp[j++] = image[i] + image[i + coefficient_count];
            temp[j++] = image[i] - image[i + coefficient_count]; //We should check to make sure bounds are legal
        }

        std::copy(temp.begin(), temp.end(), image.begin());

        coefficient_count *= 2;
    }

    return image;
}

std::vector<double> haar2d(const std::vector<uint8_t>& image_ref, int image_w, int block_w, int block_h) {

    std::vector<double> image_transform(image_ref.begin(), image_ref.end()); //Range construct for cast to double

    int num_block_columns = image_w / block_w;
    int num_block_rows = image_transform.size() / image_w / block_h;

    //std::cout << "Num Block Columns: " << num_block_columns << std::endl;
    //std::cout << "Num Block Rows: " << num_block_rows << std::endl;

    for (int block_r_index = 0; block_r_index < num_block_rows; block_r_index++) {
        for (int block_c_index = 0; block_c_index < num_block_columns; block_c_index++) {
            //Transform Block Row
            int block_origin = (block_h * block_w) * (block_r_index * num_block_columns) + (block_w * block_c_index);
            //std::cout << "\nBlock Origin: " << block_origin << std::endl;

            //std::cout << "\nBlock " << block_c_index << ", " << block_r_index << " Rows" << std::endl;
            for(int row_offset = 0; row_offset < block_h; row_offset++) {

                std::vector<double>::iterator row_origin = std::next(image_transform.begin(), block_origin + row_offset * image_w);
                std::vector<double> block_row(row_origin, std::next(row_origin, block_w));

                //print_vector(block_row, "\nBefore");

                std::vector<double> block_row_transform = haar1d(block_row);
                
                //print_vector(block_row_transform, "After");

                std::copy(block_row_transform.begin(), block_row_transform.end(), row_origin);
            }

            //Transform Block Column
            //std::cout << "\nBlock " << block_c_index << ", " << block_r_index << " Columns" << std::endl;

            for(int column_offset = 0; column_offset < block_w; column_offset++) {
                std::vector<double>::iterator column_origin = std::next(image_transform.begin(), block_origin + column_offset);
                std::vector<double> block_column_copy = extract_every_nth(column_origin, image_w, block_h);
                
                //print_vector(block_column_copy, "\nBefore");
                
                block_column_copy = haar1d(block_column_copy);
                //print_vector(block_column_copy, "After");

                copy_every_nth(block_column_copy.begin(), block_column_copy.end(), column_origin, image_w, block_h);
            }
        }
    }

    //print_vector(image_transform, "\nReal", image_w);

    return image_transform;
}

std::vector<uint8_t> ihaar2d(std::vector<double>& transform_ref, int image_w, int block_w, int block_h) {

    std::vector<double> image(transform_ref.begin(), transform_ref.end());

    int num_block_columns = image_w / block_w;
    int num_block_rows = image.size() / image_w / block_h;

    //std::cout << "Num Block Columns: " << num_block_columns << std::endl;
    //std::cout << "Num Block Rows: " << num_block_rows << std::endl;

    for (int block_r_index = 0; block_r_index < num_block_rows; block_r_index++) {
        for (int block_c_index = 0; block_c_index < num_block_columns; block_c_index++) {
            
            int block_origin = (block_h * block_w) * (block_r_index * num_block_columns) + (block_w * block_c_index);
            //std::cout << "\nBlock Origin: " << block_origin << std::endl;

            //Transform Block Row

            //std::cout << "\nBlock " << block_c_index << ", " << block_r_index << " Rows" << std::endl;
            for(int row_offset = 0; row_offset < block_h; row_offset++) {

                std::vector<double>::iterator row_origin = std::next(image.begin(), block_origin + row_offset * image_w);
                std::vector<double> block_row(row_origin, std::next(row_origin, block_w));

                //print_vector(block_row, "\nBefore");

                std::vector<double> block_row_transform = ihaar1d(block_row);
                
                //print_vector(block_row_transform, "After");

                std::copy(block_row_transform.begin(), block_row_transform.end(), row_origin);
            }

            //Transform Block Column
            //std::cout << "\nBlock " << block_c_index << ", " << block_r_index << " Columns" << std::endl;

            for(int column_offset = 0; column_offset < block_w; column_offset++) {
                std::vector<double>::iterator column_origin = std::next(image.begin(), block_origin + column_offset);
                std::vector<double> block_column_copy = extract_every_nth(column_origin, image_w, block_h);
                
                //print_vector(block_column_copy, "\nBefore");
                
                block_column_copy = ihaar1d(block_column_copy);
                //print_vector(block_column_copy, "After");

                copy_every_nth(block_column_copy.begin(), block_column_copy.end(), column_origin, image_w, block_h);
            }

        }
    }

    //print_vector(image, "\nReal", image_w);

    std::vector<uint8_t> u_image(image.begin(), image.end());

    return u_image;
}