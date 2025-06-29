//
// Created by aybehrouz on 6/15/25.
//

#ifndef CPRIOR_EVALUATOR_H
#define CPRIOR_EVALUATOR_H
#include <random>

#include "encoder/DataSet.h"
#include "encoder/Tuple.h"


namespace cprior::multinomial {
class Evaluator {
public:
    explicit Evaluator(const encoder::DataSet& data) : rnd_(rd_()), data_(data) {
    }

    std::pair<size_t, size_t> Evaluate();

    void Answer(const encoder::DataSet& query);

    double Evaluate(int trial_count) {
        std::size_t total_correct = 0, total_test = 0;
        for (int i = 0; i < trial_count; ++i) {
            auto [correct, total] = Evaluate();
            total_correct += correct;
            total_test += total;
        }
        return static_cast<double>(total_correct) / static_cast<double>(total_test);
    }

    static
    std::vector<double> EvaluateIncremental(const std::string& info_file_name,
                                            const std::string& data_file_name,
                                            const std::set<int>& target_attributes,
                                            int trials_count = 20);


private:
    std::random_device rd_;
    std::default_random_engine rnd_;
    std::discrete_distribution<> select_train_{{30, 70}};
    const encoder::DataSet& data_;
};
} // cprior::multinomial

#endif //CPRIOR_EVALUATOR_H
