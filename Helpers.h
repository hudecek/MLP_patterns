//
// Created by hudec on 10/29/17.
//

#ifndef MNIST_NN_HELPERS_H
#define MNIST_NN_HELPERS_H


#include <random>
#include <algorithm>
#include <iostream>
#include "Activations.h"

namespace Helpers {

    //sigmoid function
    float sigmoid(const float x) {
        return 1 / (1 + exp(-x));
    }

    float dsigmoid(const float value) {
        return value*(1-value);
    }

    float relu(const float x) {
        if(x < 0) {
            return 0;
        }
        return x;
    }

    float drelu(const float value) {
        if(value <= 0) {
            return 0;
        }
        return 1;
    }

    float mytanh(const float x) {
        return tanh(x);
    }

    float dmytanh(const float value) {
        return 1- pow(value, 2);
    }

    float activate(Activations activ, const float x) {
        if (activ == RELU) {
            return relu(x);
        }
        if (activ == SIGMOID) {
            return sigmoid(x);
        }
        if(activ == TANH) {
            return mytanh(x);
        }
        exit(2);
    }

    float derive(Activations activ, const float value) {
        if (activ == RELU) {
            return drelu(value);
        }
        if (activ == SIGMOID) {
            return dsigmoid(value);
        }
        if (activ == TANH) {
            return dmytanh(value);
        }
        exit(2);
    }

    void clear_weights(std::vector<std::vector<std::vector<float>>>& w) {
        for(auto& a : w) {
            for(auto& b : a) {
                for(auto& c : b) {
                    c = 0;
                }
            }
        }
    }

    void clear_layers(std::vector<std::vector<float>>& l) {
        for(auto& a : l) {
            for(auto& b : a) {
                b = 0;
            }
        }
    }


    float softmax(const float psi, const std::vector<float>& layer_psi) {
        float sum = 0;
        for(auto out : layer_psi) {
            sum += exp(out);
        }
        return float((exp(psi))/(sum));
    }

    float softmax_relu(const float psi, const std::vector<float>& layer_psi) {
        double ymax = *std::max_element(layer_psi.begin(), layer_psi.end());
        double sum = 0;
        for (float f : layer_psi)
            sum += exp(f - ymax);
        return float(exp(psi - ymax - log(sum)));
    }

    float get_random(const float a, const float b) {
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_real_distribution<> dis(a, b);
        return float(dis(gen));
    }

    std::vector<std::vector<float>> create_layer_vector(std::vector<unsigned> definition) {
        std::vector<std::vector<float>> result;
        for(auto i : definition) {
            result.emplace_back(i, 0.0);
        }
        return result;
    }

    std::vector<std::vector<std::vector<float>>> create_weights_vector(std::vector<unsigned> definition) {
        std::vector<std::vector<std::vector<float>>> result(definition.size());
        result[0] = {};
        for(unsigned l = 1; l < definition.size(); l++) {
            result[l] = std::vector<std::vector<float>>(definition[l]);
            for(unsigned i = 0; i < definition[l]; i++) {
                result[l][i] = std::vector<float>(definition[l - 1], 0.0);
            }
        }
        return result;
    }

    std::vector<std::vector<float>> add_biases(const std::vector<std::vector<float>>& first, const std::vector<std::vector<float>>& second) {
        std::vector<std::vector<float>> result = std::vector<std::vector<float>>(first.size());
        for(unsigned i = 0; i < first.size(); i++) {
            result[i] = std::vector<float>(first[i].size());
            for(unsigned j = 0; j < first[i].size(); j++) {
                result[i][j] = first[i][j] + second[i][j];
            }
        }
        return result;
    }

    std::vector<std::vector<std::vector<float>>> add_weights(const std::vector<std::vector<std::vector<float>>>& first,
                                                             const std::vector<std::vector<std::vector<float>>>& second) {
        std::vector<std::vector<std::vector<float>>> result;
        std::transform(first.begin(), first.end(), second.begin(), std::back_inserter(result),
                       [](const std::vector<std::vector<float>>& fr, const std::vector<std::vector<float>>& se) {
                           std::vector<std::vector<float>> re;
                           std::transform(fr.begin(), fr.end(), se.begin(), std::back_inserter(re),
                                  [](const std::vector<float>& f, const std::vector<float>& s) {
                                      std::vector<float> r;
                                      std::transform(f.begin(), f.end(), s.begin(),
                                                     std::back_inserter(r), std::plus<float>());
                                      return r;
                                  }
                           );
                           return re;
                       }
        );
        return result;
    }

}




#endif //MNIST_NN_HELPERS_H
