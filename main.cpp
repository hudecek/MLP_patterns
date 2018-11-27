#include <iostream>
#include <iomanip>
#include "Network.h"
#include "PrepareData.h"
#include "Activations.h"
#include "ImageData.h"
#include "png++/rgb_pixel.hpp"
#include "png++/image.hpp"

void eval_network(std::vector<unsigned>& definition, ImageData& data) {
    std::cout << "Preparing data" << std::endl;
    std::cout << "Initiating network" << std::endl;
    Network network(definition, 0.01, 0, 0, false, false, Activations::SIGMOID);
    std::cout << "Training network" << std::endl;
    network.train_patterns(1, 10000, data);
    auto e = network.output_pattern_error(data);
    std::cout << "Final error: " << e << std::endl;
}

void mnist_test() {
    auto data = ImageData("../img_data/my_test.png");
    std::cout << "Number of patterns: " << data.count_patterns() << std::endl;
    data.write_image("../img_data/test_out_img.png");
    std::vector<unsigned> definition = {100, 7, 5, 3, 100};
    eval_network(definition, data);
}



int main() {
    mnist_test();
    return 0;
}