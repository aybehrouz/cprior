//
// Created by aybehrouz on 6/15/25.
//

#include "Evaluator.h"

#include <random>

#include "InferenceEngine.h"
#include "encoder/DataSet.h"


namespace cprior::multinomial {
using namespace cprior::encoder;

using std::pair, std::set, std::vector, std::string;

pair<size_t, size_t> Evaluator::Evaluate() {
    InferenceEngine<Tuple::Instance> engine;
    vector<Tuple::Instance> test_set;


    size_t correct_count = 0;
    size_t train_size = 0;
    for (auto sample: data_) {
        if (select_train_(rnd_)) {
            ++train_size;
            //std::cout << "train:" << sample << std::endl;
            engine.AddEvidence(std::move(sample));
        } else {
            //std::cout << "test:" << sample << std::endl;
            test_set.emplace_back(std::move(sample));
        }
    }

    if (train_size == 0) return {0, 0};

    engine.ProcessEvidence();

    for (const auto& test_sample: test_set) {
        auto [possible_predictions, answer] = test_sample.ComputeTargetInstances();
        auto prediction = engine.MostProbableOutcome(possible_predictions);
        if (prediction == answer) {
            ++correct_count;
        } else {
            std::cout << "Wrong prediction:" << possible_predictions[prediction] <<
                    "instead of " << test_sample << std::endl;
        }
    }

    std::cout << "Correct:" << correct_count <<
            "\tWrong:" << test_set.size() - correct_count << std::endl;

    return {correct_count, test_set.size()};
}

vector<double> Evaluator::EvaluateIncremental(const string& info_file_name,
                                              const string& data_file_name,
                                              const set<int>& target_attributes,
                                              int trials_count) {
    vector<double> result;
    set<int> removed_attributes;

    DataSet full_data;
    auto header_tuple = full_data.ReadInfoFile(info_file_name);

    auto entry_count = header_tuple.entry_count();
    auto target = header_tuple.target();

    full_data.ReadDataFile(header_tuple, data_file_name);
    result.emplace_back(Evaluator(full_data).Evaluate(trials_count));

    for (int i = 0; i < entry_count; ++i) {
        if (target_attributes.contains(i) || target == i) continue;

        removed_attributes.insert(i);
        DataSet partial_data(removed_attributes);
        auto tuple = partial_data.ReadInfoFile(info_file_name);
        partial_data.ReadDataFile(tuple, data_file_name);

        result.emplace_back(Evaluator(partial_data).Evaluate(trials_count));
    }
    return result;
}
}
