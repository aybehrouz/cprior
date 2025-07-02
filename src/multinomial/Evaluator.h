//
// Created by aybehrouz on 6/15/25.
//

#ifndef CPRIOR_EVALUATOR_H
#define CPRIOR_EVALUATOR_H
#include <random>

#include "InferenceEngine.h"
#include "encoder/DataSet.h"
#include "encoder/Tuple.h"


namespace cprior::multinomial {
template <typename Predictor = InferenceEngine<encoder::Tuple::Instance>>
class Evaluator {
public:
    explicit Evaluator(const encoder::DataSet& data, unsigned int seed = rd_()) : rnd_(seed), data_(data) {
    }

    std::pair<size_t, size_t> Evaluate();

    void Answer(const encoder::DataSet& query);

    double Evaluate(int trial_count);

    static
    std::vector<double> EvaluateIncremental(const std::string& info_file_name,
                                            const std::string& data_file_name,
                                            const std::set<int>& target_attributes,
                                            int trials_count,
                                            unsigned int seed = rd_());


private:
    static inline std::random_device rd_;

    std::default_random_engine rnd_;
    std::discrete_distribution<> select_train_{{30, 70}};
    const encoder::DataSet& data_;
};
} // cprior::multinomial

#endif //CPRIOR_EVALUATOR_H
