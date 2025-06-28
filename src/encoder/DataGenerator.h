//
// Created by aybehrouz on 6/28/25.
//

#ifndef CPRIOR_DATA_GENERATOR_H
#define CPRIOR_DATA_GENERATOR_H
#include <random>
#include <unordered_map>
#include <set>

namespace cprior::encoder {

class DataGenerator {
public:
    DataGenerator(std::unordered_map<std::string, std::string> target_function,
                  int attr_count,
                  std::set<int> target_attr_indexes)
        : target_function_(std::move(target_function)),
          attr_count_(attr_count),
          target_attr_indexes_(std::move(target_attr_indexes)),
          rnd_(rd_()) {
    }

    std::string Generate();

    void WriteInfoFile(const std::string& filename);

    void WriteDataFile(const std::string& filename, int size = 0);

private:
    std::unordered_map<std::string, std::string> target_function_;
    int attr_count_;
    std::set<int> target_attr_indexes_;
    std::random_device rd_{};
    std::default_random_engine rnd_;
    //std::discrete_distribution a;
    std::uniform_int_distribution<> bit_selector_{0 , 1};
};

} // cprior::encoder

#endif //CPRIOR_DATA_GENERATOR_H
