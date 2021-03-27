#include "dsp.hh"

struct pixel_grayscale{
    pixel_grayscale(int value): value(value){
    }

    uint8_t value;
};

std::vector<double> haar1d(std::vector<double> image) {
    int size_original = image.size();

    if (size_original <= 1) {
        return image;
    }

    int size_temp = size_original / 2;
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

//void ihaar1d(std::vector<double> transform) {
//    int size_original = transform.size();
//}

//void haar2d() {

//}