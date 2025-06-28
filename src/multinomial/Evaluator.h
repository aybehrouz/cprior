//
// Created by aybehrouz on 6/15/25.
//

#ifndef CPRIOR_EVALUATOR_H
#define CPRIOR_EVALUATOR_H
#include "encoder/DataSet.h"
#include "encoder/Tuple.h"


namespace cprior::multinomial {
class Evaluator {
public:
    void Evaluate(const encoder::DataSet& data);

    [[nodiscard]]
    double accuracy() const {
        return static_cast<double>(total_correct_count_) / static_cast<double>(total_test_count_);
    }
private:
    std::size_t total_correct_count_ = 0;
    std::size_t total_test_count_ = 0;

};
} // cprior::multinomial

#endif //CPRIOR_EVALUATOR_H
