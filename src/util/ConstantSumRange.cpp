//
// Created by aybehrouz on 10/25/24.
//

#include "ConstantSumRange.h"

namespace cprior::util {
ConstantSumRange::ConstantSumRange(int sum_start, int sum_end, int number_of_elements)
    : begin_(number_of_elements, 0), end_(number_of_elements, 0) {
    begin_[number_of_elements - 1] = sum_start;
    end_[number_of_elements - 1] = sum_end + 1;
}

ConstantSumRange::Iterator& ConstantSumRange::Iterator::operator++() {
    auto& last = current_[current_.size() - 1];
    int last_non_zero = int(current_.size()) - 1;
    for (; last_non_zero > 0 && current_[last_non_zero] == 0; --last_non_zero) {}
    if (last_non_zero == 0) {
        last = current_[0] + 1;
        current_[0] = 0;
    } else {
        const auto remain = current_[last_non_zero] - 1;
        current_[last_non_zero] = 0;
        last = remain;
        ++current_[last_non_zero - 1];
    }
    ++current_index_;
    return *this;
}
} // cprior::util
