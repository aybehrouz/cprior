#include "../test/tools/DecisionTreePredictor.h"
#include "encoder/DataGenerator.h"
#include "encoder/DataSet.h"
#include "multinomial/Evaluator.h"


namespace cprior::test {
class DecisionTreePredictor;
}

using namespace cprior::multinomial;
using namespace cprior::encoder;
using namespace cprior::test;


int main() {
    const std::set target_attributes{0, 2, 5};
    const std::string info = "gen.info", data = "gen.data";
    constexpr int attr_count = 10, data_set_size = 20;
    constexpr int trial_count = 200;

    DataGenerator dg({
                         {"000", "A+"}, {"001", "B"}, {"010", "C"}, {"011", "D"},
                         {"100", "AA"}, {"101", "BB"}, {"110", "A+"}, {"111", "DD"}
                     }
                     , attr_count, target_attributes);

    dg.WriteInfoFile(info);
    dg.WriteDataFile(data, data_set_size);

    std::random_device seeder;
    auto seed = seeder();

    auto accuracy = Evaluator<DecisionTreePredictor>
            ::EvaluateIncremental(info, data, target_attributes, trial_count, seed);
    std::cout << "c(" << accuracy[0];

    for (int i = 1; i < accuracy.size(); ++i) {
        std::cout << "," << accuracy[i];
    }
    std::cout << ")" << std::endl;


    accuracy = Evaluator<>
            ::EvaluateIncremental(info, data, target_attributes, trial_count, seed);
    std::cout << "c(" << accuracy[0];

    for (int i = 1; i < accuracy.size(); ++i) {
        std::cout << "," << accuracy[i];
    }
    std::cout << ")" << std::endl;

    return 0;
}


/*
#include "../test/tools/DecisionTreePredictor.h"
#include "encoder/DataGenerator.h"
#include "encoder/DataSet.h"

using namespace cprior::encoder;
using namespace cprior::test;

int main() {
    const std::set target_attributes{0, 2, 6};
    const std::string info = "gen.info", data = "gen.data", test = "test.data";
    constexpr int attr_count = 15, data_set_size = 120;
    constexpr int trial_count = 300;

    DataGenerator dg({
                         {"000", "A+"}, {"001", "B"}, {"010", "C"}, {"011", "D"},
                         {"100", "AA"}, {"101", "BB"}, {"110", "A+"}, {"111", "DD"}
                     }
                     , attr_count, target_attributes);

    dg.WriteInfoFile(info);
    dg.WriteDataFile(data, data_set_size);
    dg.WriteDataFile(test, 10);

    DataSet train_data;
    auto t = train_data.ReadInfoFile(info);
    train_data.ReadDataFile(t, data);

    DecisionTreePredictor predictor;
    for (auto&& datum: train_data) {
        predictor.AddEvidence(std::move(datum));
    }

    predictor.ProcessEvidence();

    DataSet test_data;
    test_data.ReadDataFile(t, test);
    for (auto&& datum: test_data) {
        predictor.MostProbableOutcome({datum});
    }

    return 0;
}
*/
/*

#include "../test/tools/DecisionTreePredictor.h"

int main() {
    arma::mat dataset;
    mlpack::data::DatasetInfo info;
    mlpack::data::Load(DecisionTreePredictor::kTrainFileName, dataset, info, true);


    arma::Row<size_t> labels;
    mlpack::data::Load(DecisionTreePredictor::kLabelFileName, labels, true);
    // Print information about the data.
    std::cout << "The data has: " << std::endl;
    std::cout << dataset << std::endl;
    std::cout << info.Dimensionality() << std::endl;
    std::cout << " - " << dataset.n_cols << " points." << std::endl;
    std::cout << " - " << dataset.n_rows << " dimensions." << std::endl;

    std::cout << "The labels have: " << std::endl;
    std::cout << " - " << labels.n_elem << " labels." << std::endl;
    std::cout << " - A maximum label of " << labels.max() << "." << std::endl;
    std::cout << " - A minimum label of " << labels.min() << "." << std::endl;


    mlpack::tree::DecisionTree tree; // Step 1: create model.
    tree.Train(dataset, info, labels, 7); // Step 2: train model.

    arma::Row<size_t> predictions;
    arma::mat test_set;
    mlpack::data::Load("bin.test.arff", test_set, info, true);

    tree.Classify(test_set, predictions); // Step 3: classify points.

    std::cout << tree.NumChildren() << std::endl;
    std::cout << "Predictions: " << predictions << std::endl;
    return 0;
}
*/
