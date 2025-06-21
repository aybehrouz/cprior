//
// Created by aybehrouz on 6/14/25.
//

#include "DataSet.h"

namespace cprior::encoder {

using namespace std;

DataSet::DataSet(const Tuple& tuple, const std::string& data_file_name) {
    ifstream data_file(data_file_name);
    if (!data_file.is_open()) throw runtime_error("Could not open file " + data_file_name);

    string line;
    const regex reg("(\\s|,)+");
    while (getline(data_file, line)) {
        if (line.empty()) continue;
        auto token_iter = sregex_token_iterator(line.begin(), line.end(), reg, -1);
        vector<string> tokens(token_iter, {});
        data_.emplace_back(tuple, tokens);
    }
}

Tuple DataSet::ReadInfoFile(const std::string& info_file_name) {
    ifstream info_file(info_file_name);
    if (!info_file.is_open()) throw runtime_error("Could not open file " + info_file_name);

    int index_of_target;
    info_file >> index_of_target;
    Tuple result(index_of_target);

    const regex reg("(\\s|,)+");

    string type_specifier, attr_info;
    while (info_file >> type_specifier) {
        getline(info_file, attr_info);
        auto token_iter = sregex_token_iterator(attr_info.begin(), attr_info.end(), reg, -1);
        vector<string> tokens(++token_iter, {});
        if (type_specifier == "Nominal") {
                result.AddNominalEntry(tokens);
        } else if (type_specifier == "Ordinal") {
            result.AddOrdinalEntry(tokens);
        } else {
            throw runtime_error("Unrecognized attribute type specifier: " + type_specifier);
        }
    }

    result.close();
    return result;
}
} //