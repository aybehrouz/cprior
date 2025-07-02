//
// Created by aybehrouz on 6/15/25.
//

#include "Evaluator.h"

#include <random>

#include "InferenceEngine.h"
#include "../../test/tools/DecisionTreePredictor.h"
#include "encoder/DataSet.h"


namespace cprior::multinomial {
using namespace cprior::encoder;

using std::pair, std::set, std::vector, std::string;

template<typename Predictor>
pair<size_t, size_t> Evaluator<Predictor>::Evaluate() {
    Predictor engine;
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

    // std::cout << "Correct:" << correct_count << "\tWrong:" << test_set.size() - correct_count << std::endl;

    return {correct_count, test_set.size()};
}

template<typename Predictor>
void Evaluator<Predictor>::Answer(const DataSet& query) {
    Predictor engine;
    for (auto sample: data_) engine.AddEvidence(std::move(sample));
    engine.ProcessEvidence();
    for (const auto& query_sample: query) {
        auto [possible_predictions, _] = query_sample.ComputeTargetInstances();
        auto prediction = engine.MostProbableOutcome(possible_predictions);

        std::cout << "Prediction:" << possible_predictions[prediction] << std::endl;
    }
}

template<typename Predictor>
double Evaluator<Predictor>::Evaluate(int trial_count) {
    std::size_t total_correct = 0, total_test = 0;
    for (int i = 0; i < trial_count; ++i) {
        auto [correct, total] = Evaluate();
        total_correct += correct;
        total_test += total;
    }
    return static_cast<double>(total_correct) / static_cast<double>(total_test);
}

template<typename Predictor>
vector<double> Evaluator<Predictor>::EvaluateIncremental(const string& info_file_name,
                                                         const string& data_file_name,
                                                         const set<int>& target_attributes,
                                                         int trials_count,
                                                         unsigned int seed) {
    vector<double> result;
    set<int> removed_attributes;

    DataSet full_data;
    auto header_tuple = full_data.ReadInfoFile(info_file_name);

    auto entry_count = header_tuple.entry_count();
    auto target = header_tuple.index_of_target();

    // Tuple::ChangeMinAttributes(static_cast<int>(target_attributes.size()));

    full_data.ReadDataFile(header_tuple, data_file_name);
    result.push_back(Evaluator(full_data, seed).Evaluate(trials_count));

    for (int i = 0; i < entry_count; ++i) {
        if (target_attributes.contains(i) || target == i) continue;

        removed_attributes.insert(i);
        DataSet partial_data(removed_attributes);
        auto tuple = partial_data.ReadInfoFile(info_file_name);
        partial_data.ReadDataFile(tuple, data_file_name);

        result.push_back(Evaluator(partial_data, seed).Evaluate(trials_count));
    }

    // Tuple::ChangeMinAttributes();

    return result;
}

template class Evaluator<InferenceEngine<Tuple::Instance>>;
template class Evaluator<test::DecisionTreePredictor>;
}
