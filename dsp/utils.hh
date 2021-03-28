#ifndef SC_DSP_UTILS_H
#define SC_DSP_UTILS_H

#include <iostream>
#include <string>

template <class VectorIterator>
std::vector<typename std::iterator_traits<VectorIterator>::value_type> extract_every_nth(VectorIterator first, size_t n, size_t max) {
    std::vector<typename std::iterator_traits<VectorIterator>::value_type> ret;
    //Yeah, this isn't great but it's easier to get the type from the iterator than to let the template know that the type has an iterator

    for (int i = 0; i < max; i++) {
        ret.push_back(*(std::next(first, i*n)));
    }

    return ret;
}

template <class VectorType>
void print_vector(VectorType in, std::string desc, int width = 0){
    std::cout << desc << std::endl;
    int i = 0;

    for (int i = 0; i < in.size(); i++) {
        std::cout << in[i] << " ";
        if (width != 0) {
            if (i % width == width - 1)
                std::cout << "\n";
        }
    }

    std::cout << "\n";
}

template <class InIterator, class OutIterator>
void copy_every_nth(InIterator b, InIterator e, OutIterator r, size_t n, size_t max) {
    for (size_t i = 0; i < max; i++) {
        *r = *b++;
        std::advance(r,n);
    }
}

#endif
