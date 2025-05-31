//
// Created by aybehrouz on 5/21/25.
//

#ifndef VARIABLE_H
#define VARIABLE_H
#include <unordered_map>
#include "AttributeTuple.h"

namespace cprior::multinomial {
class Variable {
public:
    void add(Outcome&& observation, int count) {
        counts_[std::move(observation)] += count;
        total_count_ += count;
    }

    [[nodiscard]]
    std::unordered_map<Outcome, int> counts() const {
        return counts_;
    }

    [[nodiscard]]
    int total_count() const {
        return total_count_;
    }

    int num_of_reductions() const {
        return counts_.begin()->first.num_of_reductions();
    }

    double dim() const {
        return counts_.begin()->first.dim();
    }

    friend std::ostream& operator<<(std::ostream& os, const Variable& obj);

    int getCount(const Outcome& outcome) const {
        const auto& it = counts_.find(outcome);
        return it == counts_.end() ? 0 : it->second;
    }

private:
    std::unordered_map<Outcome, int> counts_;
    int total_count_ = 0;
};
} // cprior::multinomial

#endif // VARIABLE_H
