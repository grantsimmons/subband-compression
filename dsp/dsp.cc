#include "dsp.hh"

#include "utils.hh"

#include <math.h>

//================================//
//    Integer Haar Transforms     //
//================================//

std::vector<int> haar1d(std::vector<int>& image) {
    std::vector<int> image_transform(image.begin(), image.end());
    //Manual copy for clarity that function takes image and returns transform

    const int size_original = image_transform.size();

    if (size_original <= 1) {
        return image_transform;
    }

    const int size_temp = size_original / 2;

    std::vector<int> coarse_temp;
    std::vector<int> fine_temp;

    for (int i = 0; i < size_original; i += 2) {
        int fine_delta = image_transform[i+1] - image_transform[i];
        fine_temp.push_back(fine_delta);


        int coarse_avg = image_transform[i] + floor((double) fine_delta / 2);
        coarse_temp.push_back(coarse_avg);
    }

    std::copy(fine_temp.begin(), fine_temp.end(), std::next(image_transform.begin(), size_temp));
    
    coarse_temp = haar1d(coarse_temp);

    std::copy(coarse_temp.begin(), coarse_temp.end(), image_transform.begin());

    return image_transform;
}

std::vector<int> ihaar1d(std::vector<int>& image_transform) {
    std::vector<int> image = image_transform;
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
            temp[j++] = image[i] - floor((double) image[i + coefficient_count] / 2);
            temp[j++] = image[i + coefficient_count] + temp[j-1]; //We should check to make sure bounds are legal
        }

        std::copy(temp.begin(), temp.end(), image.begin());

        coefficient_count *= 2;
    }

    return image;
}

//================================//
// Floating Point Haar Transforms //
//================================//

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

void threshold_and_quantize(std::vector<double>& source, const double bin_size, const int image_w, const int block_w, const int block_h, const double threshold = 0) {
    int image_h = source.size() / image_w;
    
    for (int y = 0; y < image_h; y++) {
        for (int x = 0; x < image_w; x++) {
            if (x % block_w == 0 && y % block_h == 0) {
                //std::cout << "Skipping " << x << ", " << y << " Value: " << source[y * image_w + x] << std::endl;
                continue;
            }
            else {
                //std::cout << "Quantizing " << x << ", " << y << " Value: " << source[y * image_w + x] << std::endl;
                if (abs(source[y * image_w + x]) < threshold)
                    source[y * image_w + x] = 0;

                source[y * image_w + x] = floor(abs(source[y * image_w + x] / bin_size)) * sgn(source[y * image_w + x]);
            }
        }
    }
}

void dequantize(std::vector<double>& source, const double bin_size, const int image_w, const int block_w, const int block_h) {
    int image_h = source.size() / image_w;
    
    for (int y = 0; y < image_h; y++) {
        for (int x = 0; x < image_w; x++) {
            if (x % block_w == 0 && y % block_h == 0) {
                continue;
            }
            else {
                source[y * image_w + x] = abs(source[y * image_w + x] * bin_size) * sgn(source[y * image_w + x]);
                //vQ = sign(vI) .* (abs(vI)+.5) * T;
            }
        }
    }
}