//
// Created by hudec on 10/31/17.
//

#ifndef MNIST_NN_DATAHELPERS_H
#define MNIST_NN_DATAHELPERS_H

#include <sstream>
#include <vector>
#include <fstream>

namespace DataHelpers {

    std::vector<std::string> split(const std::string &s, char delim) {
        std::vector<std::string> result;
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            result.push_back(item);
        }
        return result;
    }

    std::vector<float> output_num_to_category(float num) {
        //std::vector<float> v = {0,0,0,0,0,0,0,0,0,0};
        std::vector<float> v = {0,0};
        auto index = int(num);
        if (index > 1) {
            v[1] = 1;
        } else if (index <= 0) {
            v[0] = 1;
        } else {
            v[index] = 1;
        }
        return v;
    }
}


#endif //MNIST_NN_DATAHELPERS_H
