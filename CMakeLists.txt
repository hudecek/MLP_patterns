cmake_minimum_required(VERSION 3.8)
project(MLP)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        MNIST_DATA/mnist_test_labels.csv
        MNIST_DATA/mnist_test_vectors.csv
        MNIST_DATA/mnist_train_labels.csv
        MNIST_DATA/mnist_train_vectors.csv
        MNIST_DATA/xor_in.csv
        MNIST_DATA/xor_out.csv
        Activations.h
        DataHelpers.h
        Helpers.h
        main.cpp
        network
        Network.cpp
        Network.h
        PrepareData.cpp
        PrepareData.h
        ImageData.cpp
        ImageData.h
        )

add_executable(MLP ${SOURCE_FILES})
target_link_libraries(MLP png)