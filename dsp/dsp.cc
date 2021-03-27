#include "dsp.hh"

struct pixel_grayscale{
    pixel_grayscale(int value): value(value){
    }

    uint8_t value;
};

std::vector<double> haar1d(std::vector<double> image) {
    const int size_original = image.size();

    if (size_original <= 1) {
        return image;
    }

    const int size_temp = size_original / 2;

    std::vector<double> coarse_temp;
    std::vector<double> fine_temp;

    for (int i = 0; i < size_original; i += 2) {
        double coarse_avg = (image[i] + image[i+1]) / 2;
        coarse_temp.push_back(coarse_avg);

        double fine_delta = image[i] - coarse_avg;
        fine_temp.push_back(fine_delta);
    }

    std::copy(fine_temp.begin(), fine_temp.end(), std::next(image.begin(), size_temp));
    
    coarse_temp = haar1d(coarse_temp);

    std::copy(coarse_temp.begin(), coarse_temp.end(), image.begin());

    return image;
}

std::vector<double> ihaar1d(std::vector<double> transform) {
    static int coefficient_count = 1;
    int size_original = transform.size();

    std::vector<double> temp;

    //Unpack Coefficients
    while(coefficient_count < size_original) {
        //Iterate Coefficients
        temp.resize(coefficient_count * 2);

        int j = 0; //Temp index

        for(int i = 0; i < coefficient_count; i++) {
            temp[j++] = transform[i] + transform[i + coefficient_count];
            temp[j++] = transform[i] - transform[i + coefficient_count]; //We should check to make sure bounds are legal
        }

        std::cout << std::endl;

        std::copy(temp.begin(), temp.end(), transform.begin());

        coefficient_count *= 2;
    }

    return transform;
}

//void haar2d() {

//}