//
// Created by aybehrouz on 9/7/25.
//

#ifndef CPRIOR_MULTI_MODEL_CONDITIONAL_H
#define CPRIOR_MULTI_MODEL_CONDITIONAL_H
#include <cassert>

#include "encoder/Tuple.h"

namespace cprior::multinomial::model {
class Conditional {
public:
    using Outcome = encoder::Tuple::Instance;


    void Add(Outcome observation, unsigned long target, int count) {
        using namespace util;

        if (count == 0) return;

        auto& stat = counts_[std::move(observation)];
        auto new_count = stat.target_counts_[target] += count;
        log_prob_ += Gamma::lgammaIntPlusHalf(new_count) - Gamma::lgammaIntPlusHalf(new_count - count);

        const int n = stat.total_count_ += count;
        const unsigned m = observation.target_cardinality();
        if (m % 2 == 0) log_prob_ += Gamma::lgammaInt(n - count + m / 2) - Gamma::lgammaInt(n + m / 2);
        else log_prob_ += Gamma::lgammaIntPlusHalf(n - count + m / 2) - Gamma::lgammaIntPlusHalf(n + m / 2);

        if (!children_.empty()) {
            auto reduced_observation = observation.ComputeReductions();
            assert(children_.size() == reduced_observation.size());
            for (int i = 0; i < children_.size(); ++i) {
                children_[i].Add(std::move(reduced_observation[i]), target, count);
            }
        }
    }

    util::HpFloat TreeWeightedSumAfter(const Outcome& observation, unsigned long target) const {
        auto [target_count, total_count] = FindCounts_(observation, target);

        util::HpFloat result = log_prob_.exp();
        result *= (target_count + 0.5) /
            (total_count + static_cast<double>(observation.target_cardinality()) / 2.0);

        const auto reduced_observation = observation.ComputeReductions();
        assert(children_.size() == reduced_observation.size());
        for (int i = 0; i < children_.size(); ++i) {
            result += children_[i].TreeWeightedSumAfter(reduced_observation[i], target);
        }
        return result;
    }

    int CreateSubTree() {
        children_ = std::vector<Conditional>(counts_.begin()->first.num_of_reductions());
        for (const auto& [outcome, stat]: counts_) {
            auto reductions = outcome.ComputeReductions();
            assert(reductions.size() == reduced_vars.size());
            for (auto [target, count]: stat.target_counts_) {
                for (int i = 0; i < children_.size(); ++i) {
                    children_[i].Add(reductions[i], target, count);
                }
            }
        }

        int node_count = 1;
        for (auto& child: children_) node_count += child.CreateSubTree();

        return node_count;
    }

    const Conditional* MostProbableChild() const {}


    void Add(Outcome observation, int count) {
        auto target = observation.target_int_value();
        observation.RemoveTarget();
        this->Add(std::move(observation), target, count);
    }

    [[nodiscard]]
    util::HpFloat TreeWeightedSumAfter(Outcome observation) const {
        auto target = observation.target_int_value();
        observation.RemoveTarget();
        return this->TreeWeightedSumAfter(observation, target);
    }

private:
    struct OutcomeStat {
        std::unordered_map<unsigned long, int> target_counts_;
        int total_count_ = 0;
    };
    std::vector<Conditional> children_;
    std::unordered_map<Outcome, OutcomeStat> counts_;
    util::HpFloat log_prob_ = 0.0;

    std::pair<int, int> FindCounts_(const Outcome& observation, unsigned long target) const {
        const auto& counts_it = counts_.find(observation);
        if (counts_it == counts_.end()) return {0, 0};

        const auto& target_it = counts_it->second.target_counts_.find(target);
        if (target_it == counts_it->second.target_counts_.end()) return {0, counts_it->second.total_count_} ;
        return {target_it->second, counts_it->second.total_count_};
    }
};
} // cprior::multinomial::model
#endif // CPRIOR_MULTI_MODEL_CONDITIONAL_H
