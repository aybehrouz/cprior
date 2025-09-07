//
// Created by aybehrouz on 9/3/25.
//

#ifndef CPRIOR_MODEL_DETERMINISTIC_H
#define CPRIOR_MODEL_DETERMINISTIC_H

#include <cassert>

#include "CentralPredictive.h"
#include "Variable.h"
#include "encoder/Tuple.h"

namespace cprior::multinomial::model {
template<bool conditional>
class Deterministic {
public:
    using Outcome = encoder::Tuple::Instance;

    void Add(Outcome observation, int count) {
        auto target = observation.target_int_value();
        observation.RemoveTarget();
        this->Add(std::move(observation), target, count);
    }

    void Add(Outcome&& observation, unsigned long target, int count) {
        if (pruned_ || count == 0) return;

        using namespace util;

        auto& info = counts_[std::move(observation)];
        if (info.count == 0) {
            info = {target, count};
            if constexpr (!conditional) {
                log_prob_ += Gamma::lgammaIntPlusHalf(count) - Gamma::lgammaIntPlusHalf(0) -
                        HpFloat(observation.preimage_size()).log() * count;
            }
            log_prob_ -= std::log(observation.target_cardinality());
        } else {
            if (info.target != target) {
                pruned_ = true;
                return;
            }
            auto old_count = info.count;
            info.count += count;

            if constexpr (!conditional) {
                log_prob_ += Gamma::lgammaIntPlusHalf(info.count) - Gamma::lgammaIntPlusHalf(old_count) -
                        HpFloat(observation.preimage_size()).log() * (info.count - old_count);
            }
        }

        total_count_ += count;

        if (!children_.empty()) {
            auto reduced_observation = observation.ComputeReductions();
            assert(children_.size() == reduced_observation.size());
            for (int i = 0; i < children_.size(); ++i) {
                children_[i].Add(std::move(reduced_observation[i]), target, count);
            }
        }
    }

    [[nodiscard]]
    util::HpFloat TreeWeightedSumAfter(Outcome observation) const {
        auto target = observation.target_int_value();
        observation.RemoveTarget();
        return this->TreeWeightedSumAfter(observation, target);
    }


    [[nodiscard]]
    util::HpFloat TreeWeightedSumAfter(const Outcome& observation, unsigned long target) const {
        if (pruned_) return 0.0;

        const auto& it = counts_.find(observation);
        const int count = it == counts_.end() ? 0 : it->second.count;

        if (count > 0 && it->second.target != target) return 0.0;

        util::HpFloat result = log_prob_;

        if constexpr (!conditional) {
            double d = observation.dim();
            result += std::lgamma(d / 2) - std::lgamma(total_count_ + 1 + d / 2);
        }

        result = result.exp();

        if (count == 0) {
            result /= observation.target_cardinality();
        }

        if constexpr (!conditional) {
            result *= (count + 0.5) / observation.preimage_size();
        }

        const auto reduced_observation = observation.ComputeReductions();
        assert(children_.size() == reduced_observation.size());
        for (int i = 0; i < children_.size(); ++i) {
            result += children_[i].TreeWeightedSumAfter(reduced_observation[i], target);
        }
        return result;
    }

    int CreateSubTree() {
        if (pruned_) return 0;

        children_ = std::vector<Deterministic>(counts_.begin()->first.num_of_reductions());
        for (const auto& [outcome, info]: counts_) {
            auto reductions = outcome.ComputeReductions();
            assert(reductions.size() == reduced_vars.size());
            for (int i = 0; i < children_.size(); ++i) {
                children_[i].Add(std::move(reductions[i]), info.target, info.count);
            }
        }

        int node_count = 1;
        for (auto& child: children_) node_count += child.CreateSubTree();

        return node_count;
    }

    const Deterministic* MostProbableChild() const {}

private:
    struct OutcomeInfo {
        unsigned long target;
        int count;
    };

    bool pruned_ = false;
    std::vector<Deterministic> children_;
    std::unordered_map<Outcome, OutcomeInfo> counts_;
    util::HpFloat log_prob_ = 0.0;
    int total_count_ = 0;
};
} // cprior::multinomial::model

#endif // CPRIOR_MODEL_DETERMINISTIC_H
