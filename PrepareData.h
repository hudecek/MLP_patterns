//
// Created by hudec on 10/31/17.
//

#ifndef MNIST_NN_PREPAREDATA_H
#define MNIST_NN_PREPAREDATA_H


#include <string>
#include <vector>
#include <fstream>

class PrepareData {
public:
    explicit PrepareData(const std::string& inputs_filepath, const std::string& outputs_filepath);

    void get_metadata();

    std::vector<float> normalize_input(const std::vector<float>& input);
    void reset();
    void set_unused_indices();
    void calc_avg();
    void calc_sum();
    void calc_std();
    std::vector<std::vector<std::vector<float>>> get_next(std::ifstream& in_file, std::ifstream& out_file,
                                                          unsigned no_examples);
    bool good();

    unsigned long count = 0;

    std::ifstream inputs_file;
    std::ifstream outputs_file;

    std::ifstream inputs_test;
    std::ifstream outputs_test;

    unsigned long input_dim = 0;
    unsigned long output_dim = 0;

    unsigned long real_input_dim = 0;

    std::vector<unsigned long> sum;
    std::vector<float> average;
    std::vector<float> std;
    std::vector<bool> use_index;
};


#endif //MNIST_NN_PREPAREDATA_H
