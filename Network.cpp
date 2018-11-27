//
// Created by hudec on 10/29/17.
//

#include "Network.h"

#include "Helpers.h"
#include <memory>
#include <fstream>
#include <numeric>
#include <iomanip>


//////////////////////////////////////// Initialization

Network::Network(std::vector<unsigned> def, float learning_r, float moment, float dec, bool use_cross_ent,
                 bool use_softmax, Activations activ) :
        definition(std::move(def)), momentum(moment), learning_rate(learning_r), num_layers(definition.size()),
        cross_entropy(use_cross_ent), softmax(use_softmax), decay(dec), activation(activ) {
    init_data();
}

void Network::init_data() {
    values = Helpers::create_layer_vector(definition);
    biases = Helpers::create_layer_vector(definition);
    inner_potentials = Helpers::create_layer_vector(definition);
    previous_dbiases = Helpers::create_layer_vector(definition);
    init_weights(false, false);
}

void Network::init_weights(bool load_weights, bool save_weights) {
    weights = Helpers::create_weights_vector(definition);
    previous_dweights = Helpers::create_weights_vector(definition);
    std::fstream inweights;
    if(load_weights) {
        inweights.open(weights_file, std::fstream::in);
    } else if(save_weights) {
        inweights.open(weights_file, std::fstream::out);
    }
    for(unsigned i = 0; i < definition.size() - 1; i++) {
        float val = float(6)/(definition[i] + definition[i+1]);
        float low = -4*sqrt(val);
        float high = 4*sqrt(val);
        float r = 0;
        for(unsigned j = 0; j < definition[i]; j++) {
            for(unsigned k = 0; k < definition[i + 1]; k++) {

                if(load_weights) {
                    std::string w;
                    getline(inweights, w);
                    weights[i+1][k][j] = std::stof(w);
                }
                else {
                    r = Helpers::get_random(low, high);
                    weights[i+1][k][j] = r;
                    if(save_weights) {
                        inweights << r << "\r\n";
                    }
                }
            }
        }
    }
    if(load_weights || save_weights) {
        inweights.close();
    }
}


//////////////////////////////////// Evaluate
/*
       _____________________________________
      |""|""|""|""|""|""|""|""|""|""|""|""|"|
      |  1  2  3  4  5  6  7  8  9 10 11 12 |
      '-------------------------------------'
*/

std::vector<float> Network::get_output() {
    return values[num_layers - 1];
}


void Network::evaluate(const std::vector<float>& input, PrepareData& prep) {
    auto normalized = prep.normalize_input(input);
    set_input(normalized);
    for(unsigned i = 1; i < num_layers - 1; i++) {
        eval_layer(i);
    }
    eval_output_layer();
}

void Network::set_input(const std::vector<float>& input) {
    values[0] = input;
}

void Network::calc_inner_potential(unsigned long no_layer) {
    for(unsigned long index = 0; index < values[no_layer].size(); index++) {
        inner_potentials[no_layer][index] = 0;
        inner_potentials[no_layer][index] =
                float(std::inner_product(values[no_layer - 1].begin(), values[no_layer - 1].end(),
                                         weights[no_layer][index].begin(), 0.0));
        inner_potentials[no_layer][index] += biases[no_layer][index];
    }
}

void Network::eval_layer(unsigned long no_layer) {
    calc_inner_potential(no_layer);
    std::transform(inner_potentials[no_layer].begin(), inner_potentials[no_layer].end(), values[no_layer].begin(),
                   [this](const float& ip) {
                       return Helpers::activate(activation, ip);
                   });
}

void Network::eval_output_layer() {
    if(softmax) {
        calc_inner_potential(num_layers - 1);
        std::transform(inner_potentials[num_layers - 1].begin(), inner_potentials[num_layers - 1].end(),
                       values[num_layers - 1].begin(), [this](const float &ip) {
                    return Helpers::softmax(ip, inner_potentials[num_layers - 1]);
                });

    } else {
        eval_layer(num_layers - 1);
    }
}

//////////////////////////// Calc Errors

float Network::calc_error(std::ifstream& inputs, std::ifstream& outputs, PrepareData& prep) {
    float error = 0;
    unsigned c = 0;
    while(inputs.good() && outputs.good()) {
        auto batch = prep.get_next(inputs, outputs, 100);
        for(auto& data : batch) {
            evaluate(data[0], prep);
            if (cross_entropy) {
                error += calc_category_error(data[1]);
                //error += calc_cross_error(data[1]);
            } else {
                error += calc_square_error(data[1]);
            }
            c++;
        }
    }
    inputs.clear();
    outputs.clear();
    inputs.seekg(0);
    outputs.seekg(0);
    if(c == 0) {
        return 0;
    }
    return error / c;
}


float Network::calc_category_error(const std::vector<float>& expected_output) {
    auto out = get_output();
    unsigned maxPos = 0;
    float e = 1;
    for (unsigned i = 0; i < out.size(); ++i)
    {
        //nan checks
        if(out[i] == out[i]) {
            if (out[maxPos] == out[maxPos] && out[i] >= out[maxPos]) {
                maxPos = i;
                if (expected_output[i] == 1) {
                    e = 0;
                } else {
                    e = 1;
                }
            } else if(out[maxPos] != out[maxPos]){
                maxPos = i;
                e = 0;
            }
        }
    }
    return e;
}

// Square error

float Network::calc_square_error(const std::vector<float>& expected_output) {
    float e = 0;
    auto out = get_output();
    for(unsigned long i = 0; i < definition[definition.size() - 1]; i++) {
        e += 0.5*pow(expected_output[i] - out[i], 2);
    }
    return e;
}

// Cross entropy error

float Network::calc_cross_error(const std::vector<float>& expected_output) {
    float e = 0;
    auto out = get_output();
    for(unsigned long i = 0; i < definition[definition.size() - 1]; i++) {
        e -= expected_output[i]*log(out[i]) + (1-expected_output[i])*log(1-out[i]);
    }
    return e;
}


////////////////////////////// Train
/*
         o_II_-__-__-----____________ ---------------- /====================\
         I_________I__I I    870   I I     I  I     I I   I  I     oooooo II
        /-o--0-0-0-0~~~~~o=o~==~o=o~~~o==o~~~~~~o==o~~~o=o=o~~~~~~~~~~o=o=o~~
*/

void Network::train(unsigned long no_epochs, unsigned long minibatch_size, PrepareData& prep, unsigned num_run) {
    for (unsigned e = 0; e < no_epochs; e++) {
        unsigned long i = 0;
        train_out.open(train_out_file);
        unsigned long t = 0;
        auto dweights = Helpers::create_weights_vector(definition);
        auto dbiases = Helpers::create_layer_vector(definition);
        auto dbiases_k = Helpers::create_layer_vector(definition);
        auto dweights_k = Helpers::create_weights_vector(definition);
        auto dEk_o = Helpers::create_layer_vector(definition);

        while(i < num_run) {
            if(!prep.good()) {
                prep.reset();
            }
            auto batch = prep.get_next(prep.inputs_file, prep.outputs_file, minibatch_size);
            for(auto& data : batch) {
                evaluate(data[0], prep);
                Helpers::clear_layers(dbiases_k);
                Helpers::clear_weights(dweights_k);
                Helpers::clear_layers(dEk_o);
                derive_weights_k(data[1], dbiases_k, dweights_k, dEk_o);
                dweights = Helpers::add_weights(dweights, dweights_k);
                dbiases = Helpers::add_biases(dbiases, dbiases_k);
                i++;
                if(i*100 % num_run == 0) {
                    std::cout << "Training: " << i*100 / num_run << " %" << std::endl;
                }
            }
            t++;
            change_weights(dweights, dbiases, t);

            Helpers::clear_weights(dweights);
            Helpers::clear_layers(dbiases);
        }
        train_out.close();
        prep.reset();
    }
}

void Network::test(std::string& inputs_location, std::string& outputs_location, PrepareData& prep) {
    std::ifstream inputs(inputs_location);
    std::ifstream outputs(outputs_location);
    std::ofstream output_file(predictions_out_file);
    while(inputs.good() && outputs.good()) {
        auto data = prep.get_next(inputs, outputs, 100);
        for(auto& d : data) {
            evaluate(d[0], prep);
            auto out = get_output();
            output_result(output_file, out);
        }
    }
    inputs.clear();
    outputs.clear();
    inputs.seekg(0);
    outputs.seekg(0);
    std::cout << "Test error: " << calc_error(inputs, outputs, prep) << std::endl;
    output_file.close();
    outputs.close();
    inputs.close();
}





void Network::derive_weights_k(const std::vector<float>& exp_outputs, std::vector<std::vector<float>>& dbiases,
    std::vector<std::vector<std::vector<float>>>& dweights, std::vector<std::vector<float>>& dEk_o) {
    // derivative of error with regards to outputs
    derive_outputs_k(exp_outputs, dEk_o);
    //weights hidden -> output
    derive_output_weights(dweights, dbiases, dEk_o);
    derive_hidden_weights(dweights, dbiases, dEk_o);
}


void Network::derive_hidden_weights(std::vector<std::vector<std::vector<float>>>& dweights,
                                    std::vector<std::vector<float>>& dbiases,
                                    std::vector<std::vector<float>>& derived_outputs) {
    for(long l = num_layers - 3; l >= 0; l--) {
        for(unsigned i = 0; i < values[l].size(); i++) {
            for(unsigned j = 0; j < values[l + 1].size(); j++) {
                dweights[l+1][j][i] = derived_outputs[l + 1][j]*Helpers::derive(activation, values[l+1][j])
                                      *values[l][i];
                dbiases[l + 1][j] = derived_outputs[l + 1][j]*Helpers::derive(activation, values[l+1][j]);
            }
        }
    }
}


void Network::derive_output_weights(std::vector<std::vector<std::vector<float>>>& dweights,
                                    std::vector<std::vector<float>>& dbiases,
                                    std::vector<std::vector<float>>& derived_outputs) {
    for (unsigned j = 0; j < values[num_layers - 1].size(); j++) {
        for (unsigned m = 0; m < values[num_layers - 1].size(); m++) {
            for (unsigned i = 0; i < values[num_layers - 2].size(); i++) {
                //weight[num_layers - 2][i][j]

                dweights[num_layers - 1][j][i] += derived_outputs[num_layers - 1][j]
                                                  * values[num_layers - 1][j] * (1 - values[num_layers - 1][j])
                                                  * values[num_layers - 2][i];
            }
            dbiases[num_layers - 1][j] += derived_outputs[num_layers - 1][j]
                                          * values[num_layers - 1][j] * (1 - values[num_layers - 1][j]);
        }
    }
}


void Network::derive_outputs_k(const std::vector<float>& exp_outputs, std::vector<std::vector<float>> &dEk_o) {
    auto outputs = get_output();
    output_result(train_out, outputs);
    derive_outputs_help(exp_outputs, outputs, dEk_o);
}

void Network::derive_outputs_help(const std::vector<float>& exp_outputs, std::vector<float> &real_outputs,
                                  std::vector<std::vector<float>> &store_res) {
    //output layer
    std::transform(real_outputs.begin(), real_outputs.end(), exp_outputs.begin(), store_res[num_layers - 1].begin(),
                   [](float& real, const float& expected) {
                       if(real == 1) {
                           real = 0.999;
                       } else if(real == 0) {
                           real = 0.001;
                       }
                       return (real - expected);
                   });

    //under output layer
    for(unsigned j = 0; j < values[num_layers - 2].size(); j++) {
        store_res[num_layers - 2][j] = 0;
        //foreach neuron in output layer
        for(unsigned i = 0; i < values[num_layers - 1].size(); i++) {
            store_res[num_layers - 2][j] += store_res[num_layers - 1][i] *
                                            values[num_layers - 1][i] * (1 - values[num_layers - 1][i]) *
                                            weights[num_layers - 1][i][j];
        }
    }

    //other layers
    for(long l = num_layers - 3; l >= 0; l--) {
        //foreach neuron in layer
        for(unsigned j = 0; j < values[l].size(); j++) {
            store_res[l][j] = 0;
            //foreach neuron in layer above
            for(unsigned i = 0; i < values[l + 1].size(); i++) {
                store_res[l][j] += store_res[l+1][i]*Helpers::derive(activation, values[l+1][i])
                                   *weights[l+1][i][j];
            }

        }
    }
}


void Network::change_weights(std::vector<std::vector<std::vector<float>>>& dweights,
                             std::vector<std::vector<float>>& dbiases, unsigned long t) {
    float c = std::min((t/1000.0), 1.0);
    for(unsigned l = 1; l < num_layers; l++) {
        for(unsigned j = 0; j < values[l].size(); j++) {
            for(unsigned i = 0; i < values[l-1].size(); i++) {
                weights[l][j][i] -= learning_rate*dweights[l][j][i] + c*momentum*previous_dweights[l][j][i]
                                    -decay*weights[l][j][i];
                previous_dweights[l][j][i] = - learning_rate*dweights[l][j][i]
                                               - c*momentum*previous_dweights[l][j][i];
            }
            biases[l][j] -= learning_rate*dbiases[l][j] + c*momentum*previous_dbiases[l][j]
                            - decay*biases[l][j];
            previous_dbiases[l][j] = - learning_rate*dbiases[l][j] + c*momentum*previous_dbiases[l][j];
        }
    }
}



/////////////////////////////////////////// Debug

void Network::output_result(std::ofstream& file, std::vector<float>& output) {
    auto max = std::max_element(output.begin(), output.end());
    auto maxpos = std::distance(output.begin(), max);
    file << maxpos << "\r\n";
}

float Network::calc_error_short(std::ifstream& inputs, std::ifstream& outputs, PrepareData& prep) {
    float error = 0;
    unsigned c = 0;
    auto batch = prep.get_next(inputs, outputs, 100);
    for(auto& data : batch) {
        evaluate(data[0], prep);
        if (cross_entropy) {
            error += calc_category_error(data[1]);
            //error += calc_cross_error(data[1]);
        } else {
            error += calc_square_error(data[1]);
        }
        c++;
    }
    inputs.clear();
    outputs.clear();
    inputs.seekg(0);
    outputs.seekg(0);
    if(c == 0) {
        return 0;
    }
    return error / c;
}

void Network::train_patterns(double learning_rate, unsigned long num_run, ImageData& data) {
    unsigned long i = 0;
    unsigned long t = 0;
    auto dweights = Helpers::create_weights_vector(definition);
    auto dbiases = Helpers::create_layer_vector(definition);
    auto dbiases_k = Helpers::create_layer_vector(definition);
    auto dweights_k = Helpers::create_weights_vector(definition);
    auto dEk_o = Helpers::create_layer_vector(definition);

    while(i < num_run) {
        auto ins = data.get_random_patterns();
        float min_e =  -1;
        auto min_pattern = ins[0];
        for (auto &input : ins) {
            pattern_evaluate(input);
            auto e = calc_square_error(input);
            if (e < min_e || min_e < 0) {
                min_e = e;
                min_pattern = input;
            }
        }
        pattern_evaluate(min_pattern);

        //debug
        auto no_zeros = std::count(min_pattern.begin(), min_pattern.end(), 0);

        Helpers::clear_layers(dbiases_k);
        Helpers::clear_weights(dweights_k);
        Helpers::clear_layers(dEk_o);
        derive_weights_k(min_pattern, dbiases_k, dweights_k, dEk_o);
        dweights = Helpers::add_weights(dweights, dweights_k);
        dbiases = Helpers::add_biases(dbiases, dbiases_k);
        change_weights(dweights, dbiases, t);
        Helpers::clear_weights(dweights);
        Helpers::clear_layers(dbiases);
        i++;
        // output every 10%
        if ((i+1)*10 % num_run == 0) {
            auto e = output_pattern_error(data);
            std::cout << (i+1)*100/num_run << "%: error: " << e << std::endl;
            std::string path = "../img_data/trainLargeNetwork-" + std::to_string(i) + ".png";
            data.write_network_image(this, path);
        }
    }
}

void Network::pattern_evaluate(std::vector<float> v) {
    set_input(v);
    for(unsigned i = 1; i < num_layers - 1; i++) {
        eval_layer(i);
    }
    eval_output_layer();
}

float Network::output_pattern_error(ImageData& data) {
    float total_e = 0;
    float best_patterns[200][100];
    for(int i = 0; i < 200; i++) {
        auto ins = data.get_pattern_at(i);
        float min_e =  -1;
        for (auto &input : ins) {
            pattern_evaluate(input);
            auto output = get_output();
            auto e = calc_square_error(input);
            if (e < min_e || min_e < 0) {
                min_e = e;
                for(int k = 0; k < 100; k++)
                best_patterns[i][k] = output[k];
            }
        }
        total_e += min_e;
    }
    data.count_various_patterns(best_patterns);
    return total_e;
}

