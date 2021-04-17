#ifndef SC_FILEIO_H_
#define SC_FILEIO_H_

#include <bitset>

#include "utils.hh"

template <typename T> struct canonical_huffman_table;

struct image_header {
    uint16_t x,y;
    uint8_t block_size, max_bit_length;

    union {
        struct {
            //unsigned char unused : 4;
            //unsigned char unified : 1; //Metadata contains unified Huffman Table
            //unsigned char leveled : 1; //Data is structured Hierarchically
            uint8_t unused : 4;
            uint8_t lossless : 1;
            uint8_t datatype : 1;
        } flag_data;
        uint8_t flags;
    };

    image_header(uint16_t x, uint16_t y, uint8_t block_size, uint8_t max_bit_length, bool lossless, int datatype) :
        x(x), y(y), block_size(block_size), max_bit_length(max_bit_length) {
            flag_data.lossless = lossless ? 1 : 0;
            flag_data.datatype = datatype > 0 ? 1 : 0;
            flag_data.unused = 0;
    }

    image_header() {
    }

    void print() {
        std::cout << "X Resolution: " << (int) x << std::endl;
        std::cout << "Y Resolution: " << (int) y << std::endl;
        std::cout << "Block Size: " << (int) block_size << std::endl;
        std::cout << "Max Bit Length: " << (int) max_bit_length << std::endl;
        std::cout << "Flags: " << std::hex << (int) flags << std::dec << std::endl;
    }
};

class io_write_buf {
    private:

        unsigned char buf; //Contains next byte to write to file
        int num_bits_written; //Tracks current placement in byte buffer
        std::string file_name;
        std::ofstream out;

        void write_and_flush() {
            std::cout << "Writing byte " << (int) buf << std::endl;
            out.write((char*) &buf,1);
            buf = 0x0;
            num_bits_written = 0;
        }

    public:

        io_write_buf(std::string out_file) : file_name(out_file), buf(0), num_bits_written(0) {
            out = std::ofstream(out_file);
        }

        ~io_write_buf() {
            this->close();
        }

        template <typename T>
        void write_bits(T msg, int bit_length) {
            for(int i = bit_length - 1; i >= 0; i--){
                buf |= ((msg & (1 << i)) ? 1 : 0) << (7 - num_bits_written);
                num_bits_written++;
                if(num_bits_written == 8) {
                    write_and_flush();
                }
            }
        }

        void write_header(image_header& header) {
            std::cout << "Writing " << sizeof(decltype(header.x)) * CHAR_BIT << " bits for header.x" << std::endl;
            write_bits<decltype(header.x)>(header.x,sizeof(decltype(header.x)) * CHAR_BIT);
            std::cout << "Writing " << sizeof(decltype(header.y)) * CHAR_BIT << " bits for header.y" << std::endl;
            write_bits<decltype(header.y)>(header.y,sizeof(decltype(header.y)) * CHAR_BIT);
            std::cout << "Writing " << sizeof(decltype(header.block_size)) * CHAR_BIT << " bits for header.block_size" << std::endl;
            write_bits<decltype(header.block_size)>(header.block_size,sizeof(decltype(header.block_size)) * CHAR_BIT);
            std::cout << "Writing " << sizeof(decltype(header.max_bit_length)) * CHAR_BIT << " bits for header.max_bit_length" << std::endl;
            write_bits<decltype(header.max_bit_length)>(header.max_bit_length,sizeof(decltype(header.max_bit_length)) * CHAR_BIT);
            std::cout << "Writing " << sizeof(decltype(header.flag_data)) * CHAR_BIT << " bits for header.flag_data" << std::endl;
            write_bits<decltype(header.flags)>(header.flags,sizeof(decltype(header.flags)) * CHAR_BIT);
        }

        template <typename T>
        void write_canonical_huffman_table(canonical_huffman_table<T>& table) {
            for (uint8_t bit_length = 1; bit_length <= table.max_bits; bit_length++) {
                if(table.metadata.find(bit_length) != table.metadata.end()) {
                    write_bits<uint16_t>(table.metadata[bit_length], 16);
                }
                else {
                    write_bits<uint16_t>(0, 16);
                }
            }

            for (auto value : table.canonical_table) {
                write_bits<T>(value.first, sizeof(T) * CHAR_BIT);
            }
        }

        template <typename U>
        void write_data(std::vector<U> data) {
            for (auto value : data) {
                write_bits<U>(value, bits_required(value));
            }

        }

        void close() {
            if (num_bits_written > 0) {
                write_and_flush();
            }
            out.close();
        }

};

template <typename T>
class io_read_buf {
    private:

        unsigned char buf; //Contains next byte to write to file
        int num_bits_read; //Tracks current placement in byte buffer
        std::string file_name;
        std::ifstream in;

        void read_byte() {
            if (in.is_open()) {
                if(in.read((char*) &buf,1)) {
                //std::cout << "Reading byte: " << (uint32_t) buf << std::endl;
                num_bits_read = 0;
                }
                else {
                    std::cout << "ERROR: File read exceeds EOF" << std::endl;
                    exit(1);
                }
            }
            else {
                std::cout << "ERROR: File \"" << file_name << "\" not opened!" << std::endl;  
            }
        }

    public:

        image_header header;
        canonical_huffman_table<T> canon;

        io_read_buf(std::string in_file) : file_name(in_file), buf(0), num_bits_read(8) {
            in = std::ifstream(in_file, std::ios::binary);
        }

        ~io_read_buf() {
            in.close();
        }

        template <typename X>
        X read_bits(int bit_length) {
            //std::cout << "Reading " << bit_length << " bits from \"" << file_name << "\"" << std::endl;
            X ret_val = 0;
            for (int i = bit_length - 1; i >= 0; i--) {
                if (num_bits_read == 8) {
                    read_byte();
                }
                ret_val |= ((buf & (1 << (7 - num_bits_read))) ? 1 : 0) << i;
                num_bits_read++;
            }
            //std::cout << "Value Read: " << (int) ret_val << std::endl;
            
            return ret_val;
        }

        image_header& read_header() {
            std::cout << "Reading " << sizeof(decltype(header.x)) * CHAR_BIT << " bits for header.x" << std::endl;
            header.x = read_bits<decltype(header.x)>(sizeof(decltype(header.x)) * CHAR_BIT);
            std::cout << "Reading " << sizeof(decltype(header.y)) * CHAR_BIT << " bits for header.y" << std::endl;
            header.y = read_bits<decltype(header.y)>(sizeof(decltype(header.y)) * CHAR_BIT);
            std::cout << "Reading " << sizeof(decltype(header.block_size)) * CHAR_BIT << " bits for header.block_size" << std::endl;
            header.block_size = read_bits<decltype(header.block_size)>(sizeof(decltype(header.block_size)) * CHAR_BIT);
            std::cout << "Reading " << sizeof(decltype(header.max_bit_length)) * CHAR_BIT << " bits for header.max_bit_length" << std::endl;
            header.max_bit_length = read_bits<decltype(header.max_bit_length)>(sizeof(decltype(header.max_bit_length)) * CHAR_BIT);
            std::cout << "Reading " << sizeof(decltype(header.flag_data)) * CHAR_BIT << " bits for header.flag_data" << std::endl;
            header.flags = read_bits<decltype(header.flags)>(sizeof(decltype(header.flag_data)) * CHAR_BIT);
            
            return header;
        }

        //template <typename T>
        canonical_huffman_table<T>& read_canonical_huffman_table() {
            for (int bit_length = 1; bit_length <= header.max_bit_length; bit_length++) {
                canon.metadata[bit_length] = read_bits<uint16_t>(16);
            }

            uint32_t canonical_index = 0;
            for(auto entry : canon.metadata) {
                std::cout << "Bit length: " << (int) entry.first << " Recovered Value: " << entry.second << std::endl;
                for(int i = 0; i < entry.second; i++) {
                    T symbol = read_bits<T>(sizeof(T) * CHAR_BIT);
                    canon.canonical_table.push_back(std::pair<T,uint32_t>(symbol, entry.first));
                    canon.translation_map[symbol] = canonical_index;
                    canon.inverse_map[canonical_index] = symbol;
                    //std::cout << "Pushing value " << test << " for Bit length " << (int) entry.first << " with canonical index " << canonical_index << std::endl;
                    canonical_index++;
                }
                canonical_index <<= 1;
            }
            //for (auto value : table.canonical_table) {
            //    write_bits<T>(value.first, sizeof(T) * CHAR_BIT);
            //}

            return canon;
        }

        std::vector<T> read_data() {
            std::vector<T> ret;
            uint32_t test_value;
            uint8_t counter = 0;
            for(int y = 0; y < header.y; y++) {
                for(int x = 0; x < header.x; x++) {
                    test_value = read_bits<uint32_t>(1);
                    counter = 1;
                    while(canon.inverse_map.find(test_value) == canon.inverse_map.end()) {
                        test_value = (test_value << 1) | read_bits<uint32_t>(1);
                        counter++;
                        if(counter > header.max_bit_length) {
                            std::cout << "ERROR: Value exceeds maximum bit length. You messed up." << std::endl;
                        }
                    }
                    if(canon.inverse_map.find(test_value) != canon.inverse_map.end()) {
                        std::cout << "Found canonical value: " << test_value << " Mapping: " << canon.inverse_map[test_value] << std::endl;    
                        ret.push_back(canon.inverse_map[test_value]);
                        test_value = 0;
                        counter = 0;
                    }
                }
            }
            return ret;
        }

        void close() {
            in.close();
        }
};


#endif