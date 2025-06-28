//
// Created by aybehrouz on 6/15/25.
//

#include "Evaluator.h"

#include <random>

#include "InferenceEngine.h"
#include "encoder/DataSet.h"


namespace cprior::multinomial {
using namespace cprior::encoder;

void Evaluator::Evaluate(const DataSet& data) {
    std::random_device rd;
    std::default_random_engine rnd(rd());
    std::discrete_distribution select({30, 70});

    InferenceEngine<Tuple::Instance> engine;
    std::vector<Tuple::Instance> test_set;

    int train_size = 0;
    for (auto sample: data) {
        if (select(rnd)) {
            ++train_size;
            //std::cout << "train:" << sample << std::endl;
            engine.AddEvidence(std::move(sample));
        } else {
            //std::cout << "test:" << sample << std::endl;
            test_set.emplace_back(std::move(sample));
        }
    }

    if (train_size == 0) return;

    engine.ProcessEvidence();

    auto correct_prv_value = total_correct_count_;
    for (const auto& test_sample: test_set) {
        auto [possible_predictions, answer] = test_sample.ComputeTargetInstances();
        auto prediction = engine.MostProbableOutcome(possible_predictions);
        if (prediction == answer) {
            ++total_correct_count_;
        } else {
            std::cout << "Wrong prediction:" << possible_predictions[prediction] <<
                    "instead of " << test_sample << std::endl;
        }
    }

    total_test_count_ += test_set.size();

    std::cout <<"Correct:" << total_correct_count_ - correct_prv_value <<
        "\tWrong:" << test_set.size() - (total_correct_count_ - correct_prv_value) << std::endl;
}
}
