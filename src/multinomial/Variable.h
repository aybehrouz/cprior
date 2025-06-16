//
// Created by aybehrouz on 5/21/25.
//

#ifndef VARIABLE_H
#define VARIABLE_H
#include <unordered_map>
#include "util/HpFloat.h"

namespace cprior::multinomial {
template<class Outcome>
concept Reducable = requires(std::ostream& os, Outcome const obj)
{
    { obj.ComputeReductions() } -> std::convertible_to<std::vector<Outcome> >;
    { obj.num_of_reductions() } -> std::convertible_to<util::HpFloat>;
    { obj.group_size() } -> std::convertible_to<util::HpFloat>;
    { obj.dim() } -> std::convertible_to<util::HpFloat>;
    { std::hash<Outcome>{}(obj) } -> std::convertible_to<std::size_t>;
    { obj == obj };
    { os << obj };
};

template<Reducable Outcome>
class Variable {
public:
    int Add(Outcome&& observation, int count) {
        total_count_ += count;
        return (counts_[std::move(observation)] += count) - count;
    }

    int Add(const Outcome& observation, int count) {
        total_count_ += count;
        return (counts_[observation] += count) - count;
    }

    std::vector<Variable> ComputeReducedVariables() const {
        if (counts_.empty()) return {};
        std::vector<Variable> reduced_vars(counts_.begin()->first.num_of_reductions());
        for (const auto& [outcome, count]: counts_) {
            auto reductions = outcome.ComputeReductions();
            assert(reductions.size() == reduced_vars.size());
            for (int i = 0; i < reduced_vars.size(); ++i) {
                reduced_vars[i].Add(std::move(reductions[i]), count);
            }
        }
        return reduced_vars;
    }

    [[nodiscard]]
    int total_count() const {
        return total_count_;
    }

    [[nodiscard]]
    const std::unordered_map<Outcome, int>& counts() const {
        return counts_;
    }

    [[nodiscard]]
    bool empty() const {
        return counts_.empty();
    }

    double dim() const {
        return counts_.begin()->first.dim();
    }

    unsigned long long_dim() const {
        return std::lround(this->dim());
    }

    friend std::ostream& operator<<(std::ostream& os, const Variable& obj) {
        os << "[";
        for (const auto& [outcome, count]: obj.counts_) {
            os << outcome << " = " << count << " ";
        }
        return os << "]";
    }

    int count(const Outcome& outcome) const {
        const auto& it = counts_.find(outcome);
        return it == counts_.end() ? 0 : it->second;
    }

private:
    std::unordered_map<Outcome, int> counts_;
    int total_count_ = 0;
};
} // cprior::multinomial

#endif // VARIABLE_H
