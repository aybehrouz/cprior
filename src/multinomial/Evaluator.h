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
    explicit Evaluator(const encoder::DataSet& data,
                       size_t train_size,
                       size_t test_size,
                       unsigned int seed = rd_())
        : data_(data), train_size_(train_size), test_size_(test_size), rnd_(seed) {
        if (data.size() < train_size + test_size) throw std::invalid_argument("data set is too small");

        double test_prob = static_cast<double>(test_size_) / data_.size();
        double train_prob = static_cast<double>(train_size_) / data_.size();
        double exclude_prob = 1 - test_prob + train_prob;
        select_ = std::discrete_distribution{
                {
                    test_prob + 0.1 * exclude_prob,
                    train_prob + 0.1 * exclude_prob,
                    0.8 * exclude_prob
                }
        };
    }

    explicit Evaluator(const encoder::DataSet& data,
                       unsigned int seed = rd_())
        : data_(data), train_size_(2 * data.size() / 3), test_size_(data.size() - train_size_), rnd_(seed) {
    }



    std::pair<size_t, size_t> Evaluate();

    void Answer(const encoder::DataSet& query);

    double Evaluate(int trial_count);

    static
    std::vector<double> EvaluateIncremental(const std::string& info_file_name,
                                            const std::string& data_file_name,
                                            size_t train_size, size_t test_size,
                                            const std::set<int>& target_attributes,
                                            int trials_count,
                                            unsigned int seed = rd_());

private:
    const encoder::DataSet& data_;
    size_t train_size_;
    size_t test_size_;
    std::default_random_engine rnd_;
    std::discrete_distribution<> select_;

    static inline std::random_device rd_;
};
} // cprior::multinomial

#endif //CPRIOR_EVALUATOR_H
