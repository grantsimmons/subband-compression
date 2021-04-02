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

template <typename X>
int clz_wrapper(X in) {
    int num_zeroes = sizeof(X) * CHAR_BIT - clz(in);
    num_zeroes = num_zeroes == 0 ? 1 : num_zeroes; //Need to ensure the most common case counts for 1 bit instead of zero bits
}

template <typename T>
std::pair<std::vector<std::pair<T,uint32_t>>,std::map<uint32_t,T>> generate_canonical_huffman_code(std::map<uint32_t,T>& huff_map) { //Assume that the incoming map is already sorted by huffman code

    std::vector<std::pair<T,uint32_t>> ret_vec; //I wanted to use a map, but we need a specific order with identical keys, so I'll just use a vector

    uint32_t canonical_value = 0;

    //std::unordered_map<T,uint32_t> canonical_translation_table; //Map canonical code to value
    std::map<uint32_t,T> canonical_translation_table; //Map canonical code to value
    auto map_itr = huff_map.begin();

    while(map_itr != huff_map.end()) {
        auto temp_itr = map_itr;

        uint32_t num_key_bits = clz_wrapper(map_itr->first);
        std::cout << "Finding values with indexes " << num_key_bits << " bits long" << std::endl;

        while(clz_wrapper(temp_itr->first) == num_key_bits) {
            std::cout << temp_itr->first << std::endl;
            std::advance(temp_itr,1);
        }

        std::cout << "Sorting entries..." << std::endl;

        std::vector<std::pair<T,uint32_t>> pairs;
        for (auto itr = map_itr; itr != temp_itr; std::advance(itr,1)) { //Sort by symbol frequency for tree construction
            pairs.push_back({itr->second,num_key_bits});
        }
        sort(pairs.begin(), pairs.end(), [=](std::pair<T,uint32_t>& a, std::pair<T,uint32_t>& b) {
            return a.first < b.first;
        });

        for(auto itr = pairs.begin(); itr != pairs.end(); std::advance(itr,1)) {
            if(clz_wrapper(canonical_value) < itr->second) {
                std::cout << clz_wrapper(canonical_value) << " (" << canonical_value << ") " << " < " << itr->second << "   Shifting" << std::endl;
                canonical_value = canonical_value << ((uint32_t) itr->second - (uint32_t) clz_wrapper(canonical_value));
            }
            canonical_translation_table[canonical_value] = itr->first;

            std::cout << "Value: " << itr->first << " Bit Length: " << itr->second << " Canonical Value: " << canonical_value << std::endl;
            canonical_value++;
        }

        ret_vec.insert(ret_vec.end(), pairs.begin(), pairs.end());

        map_itr = temp_itr;
    }

    std::cout << "Canonical Translation Table" << std::endl;
    for(auto pair : canonical_translation_table) {
        std::cout << "Key: " << pair.first << ", Canonical Value: " << pair.second << std::endl;
    }

    std::cout << "Entry Count: " << canonical_translation_table.size() << std::endl; 
    
    return {ret_vec, canonical_translation_table};
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