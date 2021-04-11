#ifndef SC_HUFFMAN_H_
#define SC_HUFFMAN_H_

#include "utils.hh"

#include <vector>
#include <map>
#include <deque>

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

struct Node { 
    int weight;
    bool internal;
};

struct InternalNode : Node {
    Node* left = nullptr;
    Node* right = nullptr;
    InternalNode(Node* left, Node* right) : left(left), right(right) {
        this->weight = (left ? left->weight : 0) + (right ? right->weight : 0);
        this->internal = true;
    }
};

template <typename T>
struct LeafNode : Node {
    const T symbol;
    LeafNode(T symbol, int weight) : symbol(symbol) {
        this->weight = weight;
        this->internal = false;
    }
};

template<typename T>
InternalNode* generate_huffman_tree(std::map<T,int>& source) {
    std::deque<LeafNode<T>*> leaves;
    std::deque<Node*> internal;
    InternalNode* ret;

    std::vector<std::pair<T, int>> pairs;
    for (auto itr = source.begin(); itr != source.end(); ++itr) //Sort by symbol frequency for tree construction
        pairs.push_back(*itr);

    sort(pairs.begin(), pairs.end(), [=](std::pair<T, int>& a, std::pair<T, int>& b) {
        return a.second < b.second;
    });

    for (auto entry : pairs) {
        leaves.push_back(new LeafNode<T>(entry.first, entry.second));
    }
    
    for (LeafNode<T>* entry : leaves) {
        std::cout << "Symbol: " << entry->symbol << " Weight: " << entry->weight << std::endl;
    }

    std::cout << "Leaves front: " << leaves.front()->weight << std::endl;
    std::cout << "Leaves back: " << leaves.back()->weight << std::endl;

    static int i = 0;
    while(!leaves.empty() || !internal.empty()) {
        Node* node_pair[2] = {nullptr, nullptr};
        std::cout << "Huffman Step " << i++ << std::endl;

        for(auto& ptr : node_pair) {
            if(!leaves.empty() && !internal.empty()) {
                if(leaves.front()->weight < internal.front()->weight) {
                    ptr = leaves.front();
                    leaves.pop_front();
                    std::cout << "Popping leaves cuz lower" << std::endl;
                }
                else { //Internal weight less or equal to 
                    ptr = internal.front();
                    internal.pop_front();
                    std::cout << "Popping internal cuz lower" << std::endl;
                }
            }
            else if(!leaves.empty()) {
                ptr = leaves.front();
                leaves.pop_front();
                std::cout << "Popping leaves cuz no option" << std::endl;
            }
            else if(!internal.empty()) {
                ptr = internal.front();
                internal.pop_front();
                std::cout << "Popping internal cuz no option" << std::endl;
            }
            //Shouldn't really get here...
        }

        std::cout << "node_pair: " <<  int(node_pair[0] ? node_pair[0]->weight : 0) << ", " << int(node_pair[1] ? node_pair[1]->weight : 0) << std::endl;
        
        InternalNode* comb; 

        if ((node_pair[0] ? node_pair[0]->weight : 0) < (node_pair[1] ? node_pair[1]->weight : 0)) {
            comb = new InternalNode(node_pair[0], node_pair[1]);
        }
        else {
            comb = new InternalNode(node_pair[1], node_pair[0]);
        }

        if(leaves.empty() && internal.empty()) {
            ret = comb;
            break;
        }
        //Combine pair into a single node
        internal.push_back(comb);
    }

    return ret;
}

template<typename T>
std::map<uint32_t,T> serialize_huffman_tree(Node* root, uint32_t index) {
    static std::map<uint32_t,T> ret_map;
    //static T max = 0; //TODO: Optimize bit count in final bit stream

    if(!root->internal) {
        std::cout << "Setting " << index << " to symbol " << static_cast<LeafNode<T>*>(root)->symbol << std::endl;
        ret_map[index] = static_cast<LeafNode<T>*>(root)->symbol;
    }
    else { //Internal node
        serialize_huffman_tree<T>(static_cast<InternalNode*>(root)->right, (index << 1));
        serialize_huffman_tree<T>(static_cast<InternalNode*>(root)->left, (index << 1) + 1);
    }

    return ret_map;
}

template <typename T>
struct canonical_huffman_table {
    std::vector<std::pair<T,uint32_t>> canonical_table; //Ordered vector of Huffman codes with their canonical counterparts
    std::map<T,uint32_t> translation_map; //Translates symbols to canonical codes
    std::map<uint32_t,T> inverse_map; //Translates canonical codes to symbols
    std::map<uint8_t,uint16_t> metadata; //Bit length -> Number of entries, later written directly to destination file
    uint16_t max_bits; //Maximum bit length for canonical code
    
    canonical_huffman_table() {
    }
};

template <typename T>
canonical_huffman_table<T> generate_canonical_huffman_code(std::map<uint32_t,T>& huff_map) { //Assume that the incoming map is already sorted by huffman code
    canonical_huffman_table<T> ret;

    uint32_t canonical_value = 0;

    auto map_itr = huff_map.begin();

    while(map_itr != huff_map.end()) {
        auto temp_itr = map_itr;

        uint32_t num_key_bits = bits_required(map_itr->first); //TODO: Make uint8_t to match metadata table
        uint16_t entry_counter = 0; //Count number of entries with num_key_bits index length
        std::cout << "Finding values with indexes " << num_key_bits << " bits long" << std::endl;

        while(bits_required(temp_itr->first) == num_key_bits) {
            std::cout << temp_itr->first << std::endl;
            std::advance(temp_itr,1);
            entry_counter++;
        }

        std::cout << "Found " << entry_counter << " entries with " << num_key_bits << " bits" << std::endl;
        ret.metadata[num_key_bits] = entry_counter;

        std::cout << "Sorting entries..." << std::endl;

        std::vector<std::pair<T,uint32_t>> pairs;
        for (auto itr = map_itr; itr != temp_itr; std::advance(itr,1)) { //Sort by symbol frequency for tree construction
            pairs.push_back({itr->second,num_key_bits});
        }
        sort(pairs.begin(), pairs.end(), [=](std::pair<T,uint32_t>& a, std::pair<T,uint32_t>& b) {
            return a.first < b.first;
        });

        for(auto itr = pairs.begin(); itr != pairs.end(); std::advance(itr,1)) {
            if(bits_required(canonical_value) < itr->second) {
                std::cout << bits_required(canonical_value) << " (" << canonical_value << ") " << " < " << itr->second << "   Shifting" << std::endl;
                canonical_value = canonical_value << ((uint32_t) itr->second - (uint32_t) bits_required(canonical_value));
            }
            ret.translation_map[itr->first] = canonical_value;

            std::cout << "Value: " << itr->first << " Bit Length: " << itr->second << " Canonical Value: " << canonical_value << std::endl;
            canonical_value++;
        }

        ret.canonical_table.insert(ret.canonical_table.end(), pairs.begin(), pairs.end());

        map_itr = temp_itr;
        
        ret.max_bits = num_key_bits;
    }

    //std::cout << "Canonical Translation Table" << std::endl;
    //for(auto pair : canonical_translation_table) {
    //    std::cout << "Key: " << pair.first << ", Canonical Value: " << pair.second << std::endl;
    //}

    //std::cout << "Entry Count: " << canonical_translation_table.size() << std::endl; 

    return ret;
}

template <typename T, typename U>
std::vector<U> translate_canonical(std::vector<T>& source, std::map<T,U>& translation_table) {
    std::vector<U> canon;
    for (auto src_val : source) {
        if (translation_table.find(src_val) != translation_table.end()) 
            canon.push_back(translation_table[src_val]);
        else 
            std::cout << "No canonical translation found for symbol \"" << src_val << "\"" << std::endl;
    }
    return canon;
}

void build_image_file() {
    //Build Header
        //Header and Resolution info
            //Compression Level
            //Resolution
            //Huffman Tree Key size, Value size, and Number of entries
        //Flatten Tree
    //Data
}

template<typename T>
void print2D(Node* root, int space = 0) 
{
    static int COUNT = 4;
    // Base case 
    if (!root) 
        return; 
  
    // Increase distance between levels 
    space += COUNT; 
  
    // Process right child first 
    if(root->internal == true) {
        print2D<T>(static_cast<InternalNode*>(root)->right, space); 
    }
  
    // Print current node after space count
    for (int i = COUNT; i < space; i++) 
        std::cout << " "; 
    std::cout << root->weight << "\n"; 

    if(root->internal == false) {
        for (int i = COUNT; i < space; i++) 
            std::cout << " "; 
        std::cout << "Leaf: " << static_cast<LeafNode<T>*>(root)->symbol << "\n"; 
    }
  
    // Process left child 
    if(root->internal == true)
        print2D<T>(static_cast<InternalNode*>(root)->left, space); 
}


#endif