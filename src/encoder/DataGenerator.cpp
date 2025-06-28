//
// Created by aybehrouz on 6/28/25.
//

#include <sstream>
#include <fstream>
#include "DataGenerator.h"

#include <algorithm>
#include <ranges>
#include <unordered_set>


namespace cprior::encoder {

using std::string, std::stringstream, std::ofstream;

string DataGenerator::Generate() {
    stringstream result, target_attributes;
    for (int i = 0; i < attr_count_; ++i) {
        auto generated_bit = bit_selector_(rnd_);
        result << generated_bit << ",";
        if (target_attr_indexes_.contains(i)) target_attributes << generated_bit;
    }

    result << target_function_[target_attributes.str()];

    return result.str();
}

void DataGenerator::WriteInfoFile(const std::string& filename) {
    ofstream info_file(filename);
    if (!info_file.is_open()) throw std::runtime_error("failed to create " + filename);

    info_file << attr_count_ << std::endl;
    for (int i = 0; i < attr_count_; ++i) {
        info_file << "Nominal Attr_" << i << ",0,1" << std::endl;
    }

    info_file << "Nominal Target";
    auto values_view = target_function_ | std::views::values;
    for (const auto& v: std::unordered_set(values_view.begin(), values_view.end()) ) {
        info_file << "," << v;
    }
    info_file << std::endl;
}

void DataGenerator::WriteDataFile(const std::string& filename, int size) {
    ofstream data_file(filename);
    if (!data_file.is_open()) throw std::runtime_error("failed to create " + filename);

    for (int i = 0; i < size; ++i) {
        data_file << Generate() << std::endl;
    }
}
}
