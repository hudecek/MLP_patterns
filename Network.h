//
// Created by hudec on 10/29/17.
//

#ifndef MNIST_NN_NETWORK_H
#define MNIST_NN_NETWORK_H


#include <vector>
#include <memory>
#include "PrepareData.h"
#include "Activations.h"
#include "ImageData.h"

using Layer = std::vector<float>;
using Weights = std::vector<std::vector<float>>;

class Network {
public:
    Network(std::vector<unsigned> def, float learning_r, float moment, float decay, bool use_cross_entropy,
            bool use_softmax, Activations activ);

    void evaluate(const std::vector<float>& input, PrepareData& prep);
    std::vector<float> get_output();
    float calc_error(std::ifstream& inputs, std::ifstream& outputs, PrepareData& prep);
    void train(unsigned long no_epochs, unsigned long minibatch_size, PrepareData& prep, unsigned num_run);
    void test(std::string& inputs_location, std::string& outputs_location, PrepareData& prep);

    //my additional functions for Implemetation of Neural Networks class
    void train_patterns(double learning_rate, unsigned long num_run, ImageData& data);
    void pattern_evaluate(std::vector<float> v);
    float output_pattern_error(ImageData& data);

    std::vector<unsigned> definition;
    float momentum = 0.5;
    float learning_rate = 0.1;
    unsigned long num_layers = 0;
    bool cross_entropy = false;
    bool softmax = false;
    float decay = 0.001;
    Activations activation;
    std::vector<Weights> weights;
    std::vector<Layer> values;
    std::vector<Layer> biases;
    std::vector<Layer> inner_potentials;

    float calc_error_short(std::ifstream& inputs, std::ifstream& outputs, PrepareData& prep);
    float calc_category_error(const std::vector<float>& expected_out);
    float calc_cross_error(const std::vector<float>& expected_out);
    float calc_square_error(const std::vector<float>& expected_out);

private:
    void init_data();
    void init_weights(bool load_weights, bool save_weights);

    void set_input(const std::vector<float>& input);
    void calc_inner_potential(unsigned long no_layer);
    void eval_layer(unsigned long layer_id);
    void eval_output_layer();

    //train functions
    void derive_weights_k(const std::vector<float>& outputs, std::vector<Layer>& dbiases,
                          std::vector<std::vector<std::vector<float>>>& dweights, std::vector<Layer>& dEk_o);

    void derive_outputs_k(const std::vector<float>& outputs, std::vector<Layer>& dEk_o);
    void derive_outputs_help(const std::vector<float>& exp_outputs, std::vector<float>& real_outputs,
                             std::vector<Layer> &store_res);
    void derive_output_weights(std::vector<Weights>& dweights, std::vector<Layer>& dbiases,
                               std::vector<Layer>& derived_outputs);
    void derive_hidden_weights(std::vector<Weights>& dweights, std::vector<Layer>& dbiases,
                               std::vector<Layer>& derived_outputs);

    void change_weights(std::vector<Weights>& dweights, std::vector<Layer>& dbiases, unsigned long t);


    //files manipulation
    void output_result(std::ofstream& file, std::vector<float>& out);

    float _lweight = -0.5;
    float _uweight = 0.5;

    std::vector<Weights> previous_dweights;
    std::vector<Layer> previous_dbiases;

    std::string train_out_file = "trainPredictions";
    std::string predictions_out_file = "actualTestPredictions";

    std::string weights_file = "../weights";

    std::ofstream train_out;
};


#endif //MNIST_NN_NETWORK_H
