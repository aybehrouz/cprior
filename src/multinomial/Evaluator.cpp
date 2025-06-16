//
// Created by aybehrouz on 6/15/25.
//

#include "Evaluator.h"

#include <random>

#include "InferenceEngine.h"
#include "encoder/DataSet.h"


namespace cprior::multinomial {
using namespace cprior::encoder;

void Evaluator::Evaluate(const std::string& data_file_name) {
    DataSet data(tuple_info_, data_file_name);

    std::random_device rd;
    std::mt19937 rnd(rd());
    std::discrete_distribution select({30, 70});

    InferenceEngine<Tuple::Instance> engine;
    std::vector<Tuple::Instance> test_set;

    int train_size = 0;
    for (auto& sample: data) {
        if (select(rnd)) {
            ++train_size;
            std::cout << "train:" << sample << std::endl;
            engine.AddEvidence(std::move(sample));
        } else {
            std::cout << "test:" << sample << std::endl;
            test_set.emplace_back(std::move(sample));
        }
    }

    if (train_size == 0) return;

    engine.ProcessEvidence();

    auto correct_prv_value = correct_count_;
    for (const auto& test_sample: test_set) {
        auto possible_predictions = test_sample.ComputeTargetInstances();
        auto prediction = engine.MostProbableOutcome(possible_predictions);
        if (prediction == 0) {
            ++correct_count_;
        } else {
            std::cout << "Wrong prediction:" << possible_predictions[prediction] <<
                    "instead of " << test_sample << std::endl;
        }
    }

    test_count_ += test_set.size();

    std::cout <<"Correct:" << correct_count_ - correct_prv_value <<
        "\tWrong:" << test_set.size() - (correct_count_ - correct_prv_value) << std::endl;
}
}
