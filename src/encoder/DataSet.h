//
// Created by aybehrouz on 6/14/25.
//

#ifndef CPRIOR_DATASET_H
#define CPRIOR_DATASET_H
#include <fstream>
#include <iostream>
#include <regex>
#include <set>
#include <tuple>

#include "Tuple.h"

namespace cprior::encoder {

class DataSet {
public:
    explicit DataSet(std::set<int> ignored_entries = {}) : ignored_entries_(std::move(ignored_entries)) {};

    Tuple ReadInfoFile(const std::string& info_file_name);

    void ReadDataFile(const Tuple& tuple, const std::string& data_file_name);

    [[nodiscard]]
    std::size_t size() const {
        return data_.size();
    }

    auto begin() {
        return data_.begin();
    }

    auto end() {
        return data_.end();
    }

    auto begin() const {
        return data_.cbegin();
    }


    auto end() const {
        return data_.cend();
    }

private:
    std::set<int> ignored_entries_;
    std::vector<Tuple::Instance> data_{};
};


} // namespace cprior::encoder

#endif //CPRIOR_DATASET_H
