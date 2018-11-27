//
// Created by hudec on 11/12/18.
//

#include <set>
#include <sstream>
#include <algorithm>
#include "ImageData.h"
#include "png++/image.hpp"
#include "png++/rgb_pixel.hpp"
#include "Network.h"

ImageData::ImageData(std::string filePath) {
    png::image<png::rgb_pixel> image(filePath);

    for(size_t x = 0; x < 100; x++) {
        for(size_t y = 0; y < 200; y++) {
            if(image.get_pixel(x, y).red > 128) {
                patterns[x/10 + (y/10)*10][(y%10)*10 + x%10] = 0;
            } else {
                patterns[x/10 + (y/10)*10][(y%10)*10 + x%10] = 1;
            }
        }
    }
    load_shifts();
}

void output_pattern(float p [100]) {
    for(int i = 0; i < 100; i++) {
        std::cout << p[i];
    }
    std::cout << std::endl;
}


void ImageData::load_shifts() {
    for(int i = 0; i < 200; i++) {
        //output default pattern
        //std::cout << "Pattern #" << i << std::endl;
        //output_pattern(patterns[i]);

        //original
        for(int x = 0; x < 100; x++) {
            shifted[i][0][x] = patterns[i][x];
        }

        //one up
        for(int x = 0; x < 100; x++) {
            if(x + 10 < 100) {
                shifted[i][1][x] = patterns[i][x + 10];
            } else {
                shifted[i][1][x] = 0;
            }
        }

        //std::cout << "Pattern UP #" << i << std::endl;
        //output_pattern(shifted[i][1]);

        //two up
        for(int x = 0; x < 100; x++) {
            if(x + 20 < 100) {
                shifted[i][2][x] = patterns[i][x + 20];
            } else {
                shifted[i][2][x] = 0;
            }
        }

        //std::cout << "Pattern 2UP #" << i << std::endl;
        //output_pattern(shifted[i][2]);

        //one down
        for(int x = 0; x < 100; x++) {
            if(x >= 10) {
                shifted[i][3][x] = patterns[i][x - 10];
            } else {
                shifted[i][3][x] = 0;
            }
        }

        //std::cout << "Pattern DOWN #" << i << std::endl;
        //output_pattern(shifted[i][3]);

        //two down
        for(int x = 0; x < 100; x++) {
            if(x >= 20) {
                shifted[i][4][x] = patterns[i][x - 20];
            } else {
                shifted[i][4][x] = 0;
            }
        }

        //std::cout << "Pattern 2DWN #" << i << std::endl;
        //output_pattern(shifted[i][4]);

        //one left
        for(int x = 0; x < 100; x++) {
            if(x % 10 != 9) {
                shifted[i][5][x] = patterns[i][x + 1];
            } else {
                shifted[i][5][x] = 0;
            }
        }

        //std::cout << "Pattern L #" << i << std::endl;
        //output_pattern(shifted[i][5]);

        //two left
        for(int x = 0; x < 100; x++) {
            if(x % 10 != 8 && x % 10 != 9) {
                shifted[i][6][x] = patterns[i][x + 2];
            } else {
                shifted[i][6][x] = 0;
            }
        }

        //std::cout << "Pattern 2L #" << i << std::endl;
        //output_pattern(shifted[i][6]);

        //one right
        for(int x = 0; x < 100; x++) {
            if(x % 10 != 0) {
                shifted[i][7][x] = patterns[i][x - 1];
            } else {
                shifted[i][7][x] = 0;
            }
        }

        //std::cout << "Pattern R #" << i << std::endl;
        //output_pattern(shifted[i][7]);

        //two right
        for(int x = 0; x < 100; x++) {
            if(x % 10 != 0 && x % 10 != 1) {
                shifted[i][8][x] = patterns[i][x - 2];
            } else {
                shifted[i][8][x] = 0;
            }
        }

        //std::cout << "Pattern RR #" << i << std::endl;
        //output_pattern(shifted[i][8]);

        //one up left
        for(int x = 0; x < 100; x++) {
            if(x % 10 != 9 && x + 11 < 100) {
                shifted[i][9][x] = patterns[i][x + 11];
            } else {
                shifted[i][9][x] = 0;
            }
        }

        //std::cout << "Pattern UL #" << i << std::endl;
        //output_pattern(shifted[i][9]);

        //one up right
        for(int x = 0; x < 100; x++) {
            if(x % 10 != 0 && x + 11 < 100) {
                shifted[i][10][x] = patterns[i][x + 9];
            } else {
                shifted[i][10][x] = 0;
            }
        }

        //std::cout << "Pattern UR #" << i << std::endl;
        //output_pattern(shifted[i][10]);

        //one down left
        for(int x = 0; x < 100; x++) {
            if(x % 10 != 9 && x > 9) {
                shifted[i][11][x] = patterns[i][x - 9];
            } else {
                shifted[i][11][x] = 0;
            }
        }

        //std::cout << "Pattern DL #" << i << std::endl;
        //output_pattern(shifted[i][11]);

        //one down right
        for(int x = 0; x < 100; x++) {
            if(x % 10 != 0 && x > 11) {
                shifted[i][12][x] = patterns[i][x - 11];
            } else {
                shifted[i][12][x] = 0;
            }
        }

        // two up one left
        for(int x = 0; x < 100; x++) {
            if(x % 10 != 9 && x + 21 < 100) {
                shifted[i][13][x] = patterns[i][x + 21];
            } else {
                shifted[i][13][x] = 0;
            }
        }

        //one up two left
        for(int x = 0; x < 100; x++) {
            if(x % 10 != 9 && x % 10 != 8 && x + 12 < 100) {
                shifted[i][14][x] = patterns[i][x + 12];
            } else {
                shifted[i][14][x] = 0;
            }
        }

        // two up one right
        for(int x = 0; x < 100; x++) {
            if(x % 10 != 0 && x + 20 < 100) {
                shifted[i][15][x] = patterns[i][x + 19];
            } else {
                shifted[i][15][x] = 0;
            }
        }

        //one up two right
        for(int x = 0; x < 100; x++) {
            if(x % 10 != 0  && x % 10 != 1 && x + 8 < 100) {
                shifted[i][16][x] = patterns[i][x + 8];
            } else {
                shifted[i][16][x] = 0;
            }
        }

        //two down one left
        for(int x = 0; x < 100; x++) {
            if(x % 10 != 9 && x > 19) {
                shifted[i][17][x] = patterns[i][x - 19];
            } else {
                shifted[i][17][x] = 0;
            }
        }

        //two down one right
        for(int x = 0; x < 100; x++) {
            if(x % 10 != 0 && x > 20) {
                shifted[i][18][x] = patterns[i][x - 21];
            } else {
                shifted[i][18][x] = 0;
            }
        }

        // one down two left
        for(int x = 0; x < 100; x++) {
            if(x % 10 != 9 && x % 10 != 8 && x > 9) {
                shifted[i][19][x] = patterns[i][x - 8];
            } else {
                shifted[i][19][x] = 0;
            }
        }

        // one down two right
        for(int x = 0; x < 100; x++) {
            if(x % 10 != 0  && x % 10 != 1 && x > 10) {
                shifted[i][20][x] = patterns[i][x - 12];
            } else {
                shifted[i][20][x] = 0;
            }
        }

        // two down two right
        for(int x = 0; x < 100; x++) {
            if(x % 10 != 0  && x % 10 != 1 && x > 20) {
                shifted[i][21][x] = patterns[i][x - 22];
            } else {
                shifted[i][21][x] = 0;
            }
        }

        // two down two left
        for(int x = 0; x < 100; x++) {
            if(x % 10 != 9 && x % 10 != 8 && x > 19) {
                shifted[i][22][x] = patterns[i][x - 18];
            } else {
                shifted[i][22][x] = 0;
            }
        }

        // two up two left
        for(int x = 0; x < 100; x++) {
            if(x % 10 != 9 && x % 10 != 8 && x < 77) {
                shifted[i][23][x] = patterns[i][x + 22];
            } else {
                shifted[i][23][x] = 0;
            }
        }

        // two up two right
        for(int x = 0; x < 100; x++) {
            if(x % 10 != 0 && x % 10 != 1 && x < 80) {
                shifted[i][24][x] = patterns[i][x + 18];
            } else {
                shifted[i][24][x] = 0;
            }
        }

        //std::cout << "Pattern DR #" << i << std::endl;
        //output_pattern(shifted[i][12]);
    }
}

void ImageData::write_image(const std::string& path) {
    write_image(path, patterns);
}


void ImageData::write_image(const std::string& path, const float patts[200][100]) {
    png::image<png::rgb_pixel> img(100, 200);
    for(size_t x = 0; x < 100; x++) {
        for(size_t y = 0; y < 200; y++) {
            png_byte r = 0;
            png_byte g = 0;
            png_byte b = 0;
            if(patts[x/10+(y/10)*10][(y%10)*10 + x%10] < 0.5) {
                r = 255;
                g = 255;
                b = 255;
            }
            png::rgb_pixel p(r, g, b);
            img.set_pixel(x, y, p);
        }
    }
    img.write(path);
}

std::string ImageData::float_array_to_string(const float *doubles, int size_of_array) {
    std::ostringstream oss("");
    for (int temp = 0; temp < size_of_array; temp++)
        if (doubles[temp] > 0.5)
            oss << 1;
        else
            oss << 0;
    return oss.str();
}

int ImageData::count_patterns() {
    std::string str_array[200];
    for(int i = 0; i < 200; i++) {
        str_array[i] = float_array_to_string(patterns[i], 100);
    }
    //int N = sizeof(str_array)/sizeof(str_array[0]); //Get the array size

    int count = 1;
    int rest_count = 0;
    std::sort(str_array,str_array+200); //Use the start and end like this
    for(int i = 0; i < 199; i++) {
        if(str_array[i] != str_array[i+1]) {
            count++;
        } else {
            rest_count++;
        }
    }
    std::cout << "REST: " << rest_count << std::endl;
    return count;
}

std::vector<std::vector<float>> ImageData::get_pattern_at(int index) {
    std::vector<std::vector<float>> result;
    auto x = patterns[index];
    std::vector<float> v(x, x + 100);
    result.push_back(v);
    for(int i = 0; i < 25; i++) {
        x = shifted[index][i];
        v = std::vector<float>(x, x + 100);
        result.push_back(v);
    }
    return result;
}



std::vector<std::vector<float>> ImageData::get_random_patterns() {
    int random = rand() % 200;
    return get_pattern_at(random);
}

void ImageData::write_network_image(Network* net, const std::string& path) {
    auto new_patterns = new float[200][100];
    for(int i = 0; i < 200; i++) {

        auto ins = get_pattern_at(i);
        float min_e =  -1;
        auto min_pattern = ins[0];
        for (auto &input : ins) {
            net->pattern_evaluate(input);
            auto e = net->calc_square_error(input);
            if (e < min_e || min_e < 0) {
                min_e = e;
                min_pattern = input;
            }
        }
        net->pattern_evaluate(min_pattern);
        auto out = net->get_output();
        for(int j = 0; j < 100; j ++) {
            new_patterns[i][j] = out[j];
        }
    }
    write_image(path, new_patterns);
}

void ImageData::count_various_patterns(float (*v)[100]) {
    std::string str_array[200];
    for(int i = 0; i < 200; i++) {
        str_array[i] = float_array_to_string(v[i], 100);
    }
    int count = 1;
    std::sort(str_array,str_array+200); //Use the start and end like this
    for(int i = 0; i < 199; i++) {
        if(str_array[i] != str_array[i+1]) {
            count++;
        }
    }
    std::cout << "Various patterns: " << count << std::endl;
}
