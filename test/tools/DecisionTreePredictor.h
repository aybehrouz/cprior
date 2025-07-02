//
// Created by aybehrouz on 7/2/25.
//

#ifndef CPRIOR_DECISION_TREE_PREDICTOR_H
#define CPRIOR_DECISION_TREE_PREDICTOR_H
#include <fstream>
#include <mlpack/methods/decision_tree/decision_tree.hpp>

#include "encoder/Tuple.h"

namespace cprior::test {
class DecisionTreePredictor {
public:
    static constexpr auto kTrainFileName = "dt_train_.tmp.arff";
    static constexpr auto kLabelFileName = "dt_label_.tmp.csv";

    using Datum = encoder::Tuple::Instance;

    DecisionTreePredictor();

    void AddEvidence(Datum&& observation, int count = 1);

    void ProcessEvidence();

    int MostProbableOutcome(const std::vector<Datum>& query);

private:
    bool header_added_ = false;
    std::ofstream train_file_;
    std::ofstream label_file_;
    size_t num_of_classes_ = 0;
    mlpack::tree::DecisionTree<> tree_;
    mlpack::data::DatasetInfo info_;

    static void WriteHeader_(std::ofstream& file, const Datum& datum);
};
} // namespace cprior::test

#endif //CPRIOR_DECISION_TREE_PREDICTOR_H
