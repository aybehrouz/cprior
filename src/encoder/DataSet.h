//
// Created by aybehrouz on 6/14/25.
//

#ifndef CPRIOR_DATASET_H
#define CPRIOR_DATASET_H
#include <fstream>
#include <iostream>
#include <regex>
#include <tuple>
#include <gtest/internal/gtest-port.h>

#include "Tuple.h"

namespace cprior::encoder {

class DataSet {
public:
    explicit DataSet(const Tuple& tuple, const std::string& data_file_name);

    static Tuple ReadInfoFile(const std::string& info_file_name);

    auto begin() {
        return data_.begin();
    }

    auto end() {
        return data_.end();
    }

private:
    std::vector<Tuple::Instance> data_{};
};


} // namespace cprior::encoder

#endif //CPRIOR_DATASET_H
