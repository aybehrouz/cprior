//
// Created by aybehrouz on 6/14/25.
//

#include "DataSet.h"

namespace cprior::encoder {
using namespace std;


Tuple DataSet::ReadInfoFile(const std::string& info_file_name) {
    ifstream info_file(info_file_name);
    if (!info_file.is_open()) throw runtime_error("Could not open file " + info_file_name);

    int index_of_target;
    info_file >> index_of_target;
    if (ignored_entries_.contains(index_of_target)) throw runtime_error("target is in ignore list");
    // The distance from the beginning to this iterator gives the count of elements smaller than target_number
    index_of_target -= static_cast<int>(distance(ignored_entries_.begin(),
                                                 lower_bound(ignored_entries_.begin(), ignored_entries_.end(),
                                                             index_of_target)));
    Tuple result(index_of_target);

    const regex reg("(\\s|,)+");

    string type_specifier, attr_info;
    int entry_index = 0;
    while (info_file >> type_specifier) {
        getline(info_file, attr_info);
        if (ignored_entries_.contains(entry_index++)) continue;

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

void DataSet::ReadDataFile(const Tuple& tuple, const std::string& data_file_name) {
    ifstream data_file(data_file_name);
    if (!data_file.is_open()) throw runtime_error("Could not open file " + data_file_name);

    data_.clear();
    string line;
    const regex reg("(\\s|,)+");
    while (getline(data_file, line)) {
        if (line.empty()) continue;
        vector<string> tokens;
        int entry_index = 0;
        for (auto token_iter = sregex_token_iterator(line.begin(), line.end(), reg, -1);
             token_iter != std::sregex_token_iterator();
             ++token_iter) {
            if (!ignored_entries_.contains(entry_index++)) tokens.push_back(*token_iter);
        }
        data_.emplace_back(tuple, tokens);
    }
}
} //