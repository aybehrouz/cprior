//
// Created by aybehrouz on 6/14/25.
//

#ifndef CPRIOR_DATASET_H
#define CPRIOR_DATASET_H
#include <fstream>
#include <iostream>
#include <regex>

namespace cprior::encoder {

class DataSet {
public:
    void ReadFile(const std::string& file_name) {
        std::ifstream file(file_name);
        if (!file.is_open()) throw std::runtime_error("Error opening file");

        std::string line;
        const std::regex reg("(\\s|,)+");
        while (std::getline(file, line)) {
            auto token_iter = std::sregex_token_iterator(line.begin(), line.end(), reg, -1);
            std::vector<std::string> tokens(token_iter, {});
            for (const auto& token: tokens) {
                std::cout << token << " ";
            }
            std::cout << std::endl;
        }
    }

};


} // namespace cprior::encoder

#endif //CPRIOR_DATASET_H
