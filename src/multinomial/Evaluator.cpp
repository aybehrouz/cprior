//
// Created by aybehrouz on 6/15/25.
//

#include "Evaluator.h"

#include <random>

#include "InferenceEngine.h"
#include "encoder/DataSet.h"


namespace cprior::multinomial {
using namespace cprior::encoder;

void Evaluator::Eval() {
    auto tuple_info = DataSet::ReadInfoFile("info.txt");

    DataSet data(tuple_info, "data.txt");

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

    std::cout << "accuracy:" << correct / static_cast<double>(test_set.size()) << std::endl;
}
}
