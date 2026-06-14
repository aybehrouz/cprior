//
// Created by aybehrouz on 7/2/25.
//



#include <iostream>

#include "DecisionTreePredictor.h"

namespace cprior::test {
DecisionTreePredictor::DecisionTreePredictor(): train_file_(kTrainFileName), label_file_(kLabelFileName) {
    if (!train_file_.is_open() || !label_file_.is_open()) {
        throw std::runtime_error("DecisionTree: Could not create train temporary files");
    }
}

void DecisionTreePredictor::AddEvidence(Datum&& observation, int count) {
    if (!header_added_) {
        WriteHeader_(train_file_, observation);
        num_of_classes_ = observation.target_cardinality();
        header_added_ = true;
    }
    for (int i = 0; i < count; ++i) {
        train_file_ << observation.attribute_str() << std::endl;
        label_file_ << observation.target_int_value() - 1 << std::endl;
    }
}

void DecisionTreePredictor::ProcessEvidence() {
    train_file_.close();
    label_file_.close();

    arma::mat dataset;

    mlpack::data::Load(kTrainFileName, dataset, info_, true);
    arma::Row<size_t> labels;
    mlpack::data::Load(kLabelFileName, labels, true);
/*
    // Print information about the data.
    std::cout << "The data has: " << std::endl;
    std::cout << dataset << std::endl;
    std::cout << " - " << dataset.n_cols << " points." << std::endl;
    std::cout << " - " << info_.Dimensionality() << " dimensions." << std::endl;

    std::cout << "The labels have: " << std::endl;
    std::cout << " - " << labels.n_elem << " labels." << std::endl;
    std::cout << " - A maximum label of " << labels.max() << "." << std::endl;
    std::cout << " - A minimum label of " << labels.min() << "." << std::endl;
*/
    tree_.Train(dataset, info_, labels, num_of_classes_); // train model.
}

int DecisionTreePredictor::MostProbableOutcome(const std::vector<Datum>& query) {
    auto& test_sample = query.at(0);
    arma::mat test_set(test_sample.attribute_count(), 1);
    for (int i = 0; i < test_sample.attribute_count(); ++i) {
        test_set(i, 0) = info_.MapString<double>(test_sample.attribute_str(i), i);
    }

    auto prediction = test_sample;
    prediction.set_target(1 + tree_.Classify(test_set.col(0)));

    //std::cout << "Prediction: " << prediction << std::endl;

    for (int i = 0; i < query.size(); ++i) if (query[i] == prediction) return i;
    return -1;
}

void DecisionTreePredictor::WriteHeader_(std::ofstream& file, const Datum& datum) {
    file << "#relation tmp" << std::endl;
    for (int i = 0; i < datum.attribute_count(); ++i) {
        file << "@attribute attr_" << i << " string" << std::endl;
    }
    file << std::endl;
    file << "@data" << std::endl;
}
}
