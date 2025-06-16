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

    for (auto& sample: data) {
        if (select(rnd)) {
            std::cout << "train:" << sample << std::endl;
            engine.AddEvidence(std::move(sample));
        } else {
            std::cout << "test:" << sample << std::endl;
            test_set.emplace_back(std::move(sample));
        }
    }

    engine.ProcessEvidence();

    int correct = 0;
    for (const auto& test_sample: test_set) {
        auto possible_predictions = test_sample.ComputeTargetInstances();
        auto prediction = engine.MostProbableOutcome(possible_predictions);
        if (prediction == 0) {
            ++correct;
        } else {
            std::cout << "Wrong prediction:" << possible_predictions[prediction] <<
                    "instead of " << test_sample << std::endl;
        }
    }

    std::cout << "Correct:" << correct << " || Wrong:" << test_set.size() - correct << std::endl;


    if (!test_set.empty()) {
        accuracy_ = accuracy_ * trial_count_ + correct / static_cast<double>(test_set.size());
        ++trial_count_;
        accuracy_ /= trial_count_;
    }
}
}
