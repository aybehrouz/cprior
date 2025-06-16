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
    explicit Evaluator(const std::string& info_file_name)
        : tuple_info_(encoder::DataSet::ReadInfoFile(info_file_name)) {
    }

    void Evaluate(const std::string& data_file_name);

    [[nodiscard]]
    double accuracy() const {
        return accuracy_;
    }
private:
    encoder::Tuple tuple_info_;
    double accuracy_ = 0.0;
    int trial_count_ = 0;
};
} // cprior::multinomial

#endif //CPRIOR_EVALUATOR_H
