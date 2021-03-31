#ifndef SC_HUFFMAN_H_
#define SC_HUFFMAN_H_

#include "utils.hh"

#include <vector>
#include <map>

template <typename T>
std::map<T,int> generate_frequency_table(std::vector<T>& in_vec) {
    std::map<T,int> freq_count;

    for(T val : in_vec) {

        if(freq_count.find(val) == freq_count.end()) {
            freq_count[val] = 1;
        }
        else {
            freq_count[val]++;
        }
    }

    return freq_count;
}

#endif