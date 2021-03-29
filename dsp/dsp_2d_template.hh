#ifndef SC_DSP_DSP_TEMPLATE_H_
#define SC_DSP_DSP_TEMPLATE_H_

#include "dsp.hh"
#include "utils.hh"

template <typename T>
std::vector<T> haar2d(const std::vector<uint8_t>& image_ref, const int image_w, const int block_w, const int block_h) {

    std::vector<T> image_transform(image_ref.begin(), image_ref.end()); //Range construct for cast to double

    int num_block_columns = image_w / block_w;
    int num_block_rows = image_transform.size() / image_w / block_h;

    for (int block_r_index = 0; block_r_index < num_block_rows; block_r_index++) {
        for (int block_c_index = 0; block_c_index < num_block_columns; block_c_index++) {

            //Transform Block Row
            int block_origin = (block_h * block_w) * (block_r_index * num_block_columns) + (block_w * block_c_index);

            for(int row_offset = 0; row_offset < block_h; row_offset++) {

                typename std::vector<T>::iterator row_origin = std::next(image_transform.begin(), block_origin + row_offset * image_w);
                std::vector<T> block_row(row_origin, std::next(row_origin, block_w));

                std::vector<T> block_row_transform = haar1d(block_row);

                std::copy(block_row_transform.begin(), block_row_transform.end(), row_origin);
            }

            //Transform Block Column
            for(int column_offset = 0; column_offset < block_w; column_offset++) {
                typename std::vector<T>::iterator column_origin = std::next(image_transform.begin(), block_origin + column_offset);
                std::vector<T> block_column_copy = extract_every_nth(column_origin, image_w, block_h);
                
                block_column_copy = haar1d(block_column_copy);

                copy_every_nth(block_column_copy.begin(), block_column_copy.end(), column_origin, image_w, block_h);
            }
        }
    }

    return image_transform;
}

template <typename T>
std::vector<uint8_t> ihaar2d(std::vector<T>& transform_ref, const int image_w, const int block_w, const int block_h) {

    std::vector<T> image(transform_ref.begin(), transform_ref.end());

    int num_block_columns = image_w / block_w;
    int num_block_rows = image.size() / image_w / block_h;

    for (int block_r_index = 0; block_r_index < num_block_rows; block_r_index++) {
        for (int block_c_index = 0; block_c_index < num_block_columns; block_c_index++) {
            
            int block_origin = (block_h * block_w) * (block_r_index * num_block_columns) + (block_w * block_c_index);

            //We need to keep this in the opposite order of transform because the integer transform isn't separable
            //Transform Block Column
            for(int column_offset = 0; column_offset < block_w; column_offset++) {
                typename std::vector<T>::iterator column_origin = std::next(image.begin(), block_origin + column_offset);
                std::vector<T> block_column_copy = extract_every_nth(column_origin, image_w, block_h);
                
                block_column_copy = ihaar1d(block_column_copy);

                copy_every_nth(block_column_copy.begin(), block_column_copy.end(), column_origin, image_w, block_h);
            }

            //Transform Block Row
            for(int row_offset = 0; row_offset < block_h; row_offset++) {

                typename std::vector<T>::iterator row_origin = std::next(image.begin(), block_origin + row_offset * image_w);
                std::vector<T> block_row(row_origin, std::next(row_origin, block_w));

                std::vector<T> block_row_transform = ihaar1d(block_row);

                std::copy(block_row_transform.begin(), block_row_transform.end(), row_origin);
            }
        }
    }

    std::vector<uint8_t> u_image(image.begin(), image.end());

    return u_image;
}

#endif