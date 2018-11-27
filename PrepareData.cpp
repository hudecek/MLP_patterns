//
// Created by hudec on 10/31/17.
//

#include <iostream>
#include <cmath>
#include <limits>
#include <algorithm>
#include "PrepareData.h"
#include "DataHelpers.h"


PrepareData::PrepareData(const std::string &inputs_filepath, const std::string &outputs_filepath) {
    inputs_file.open(inputs_filepath);
    outputs_file.open(outputs_filepath);
    inputs_test.open(inputs_filepath);
    outputs_test.open(outputs_filepath);
    if( !inputs_file.good() || !outputs_file.good()) {
        std::cerr << "Files are invalid!" << std::endl;
        exit(2);
    }
    get_metadata();

    calc_sum();
    calc_avg();
    calc_std();
    set_unused_indices();
}


void PrepareData::get_metadata() {
    while(inputs_file.good() && outputs_file.good()) {
        std::string in, out;
        getline(inputs_file, in);
        getline(outputs_file, out);
        if(in.length() > 0) {
            auto values = DataHelpers::split(in, ',');
            if(input_dim != 0 && values.size() != input_dim) {
                std::cout << "Incorrect input dimensions! " << input_dim << " vs " << values.size() << std::endl;
                exit(3);
            }
            input_dim = values.size();
            count++;
        }
        if(out.length() > 0) {
            auto values = DataHelpers::split(out, ',');
            if(output_dim != 0 && values.size() != output_dim) {
                std::cout << "Incorrect output dimensions! " << output_dim << " vs " << values.size() << std::endl;
                exit(3);
            }
            output_dim = values.size();
        }
    }
    reset();
}


std::vector<std::vector<std::vector<float>>> PrepareData::get_next(std::ifstream& in_file,
                                                                   std::ifstream& out_file, unsigned no_examples) {
    std::vector<std::vector<std::vector<float>>> res(no_examples);
    std::vector<std::vector<float>> result(2);
    result[0] = std::vector<float>(input_dim);
    result[1] = std::vector<float>(output_dim);
    std::string in, out;
    unsigned i = 0;
    while (in_file.good() && out_file.good() && i < no_examples) {
        getline(in_file, in);
        getline(out_file, out);
        if(!in.empty() && !out.empty()) {
            auto values = DataHelpers::split(in, ',');
            std::transform(values.begin(), values.end(), result[0].begin(), [](const std::string& val) {
                return std::stof(val);
            });

            values = DataHelpers::split(out, ',');
            for (const auto &value : values) {
                result[1] = DataHelpers::output_num_to_category(std::stof(value));
            }
            res[i++] = result;
        }
    }
    res.resize(i);
    return res;
}


void PrepareData::set_unused_indices() {
    use_index = std::vector<bool>(input_dim);
    real_input_dim = 0;
    for(unsigned i = 0; i < input_dim; i++) {
        if(sum[i] <= count / 100) {
            use_index[i] = false;
        } else {
            use_index[i] = true;
            real_input_dim++;
        }
    }
}

bool PrepareData::good() {
    return inputs_file.good() && outputs_file.good();
}

void PrepareData::calc_sum() {
    sum = std::vector<unsigned long>(input_dim, 0);
    for(unsigned j = 0; j < count; j++) {
        auto data = get_next(inputs_file, outputs_file, 1);
        for(unsigned long i = 0; i < data[0][0].size(); i++) {
            sum[i] += data[0][0][i];
        }
    }
    reset();
}

void PrepareData::calc_avg() {
    average = std::vector<float>(input_dim);
    for(unsigned long i = 0; i < input_dim; i++) {
        average[i] = sum[i]/float(count);
    }
}

void PrepareData::calc_std() {
    std = std::vector<float>(input_dim);
    for(unsigned j = 0; j < count; j++) {
        auto data = get_next(inputs_file, outputs_file, 1);
        for(unsigned long i = 0; i < data[0][0].size(); i++) {
            std[i] += pow(data[0][0][i] - average[i], 2);
        }
    }
    reset();

    for(unsigned i = 0; i < input_dim; i++) {
        std[i] = sqrt(std[i]/(count - 1));
    }
}

void PrepareData::reset() {
    inputs_file.clear();
    outputs_file.clear();
    inputs_file.seekg(0);
    outputs_file.seekg(0);
}

std::vector<float> PrepareData::normalize_input(const std::vector<float>& input) {
    auto normalized = std::vector<float>();
    for(unsigned i = 0; i < input_dim; i++) {
        if(use_index[i]) {
            float modified = (input[i] - average[i]) / std[i];
            normalized.push_back(modified);
        }
    }
    return normalized;
}