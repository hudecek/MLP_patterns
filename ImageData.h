//
// Created by hudec on 11/12/18.
//

#ifndef MLP_IMAGEDATA_H
#define MLP_IMAGEDATA_H

#include <string>
#include <vector>

class Network;
class ImageData {
public:
    ImageData(std::string filePath);
    int count_patterns();
    void write_image(const std::string& path, const float patts[200][100]);
    void write_image(const std::string& path);
    void write_network_image(Network* net, const std::string& path);
    std::string float_array_to_string(const float *doubles, int size_of_array);
    void count_various_patterns(float new_patterns[200][100]);
    std::vector<std::vector<float>> get_random_patterns();
    std::vector<std::vector<float>> get_pattern_at(int index);
private:
    void load_shifts();
    float patterns[200][100];
    float shifted[200][50][100];
};


#endif //MLP_IMAGEDATA_H
