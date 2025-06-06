//
// Created by aybehrouz on 5/23/25.
//

#ifndef INFERENCE_NODE_H
#define INFERENCE_NODE_H
#include <ostream>
#include <vector>
#include <cassert>

#include "Variable.h"
#include "util/Gamma.h"
#include "util/HpFloat.h"


namespace cprior::multinomial {
template<Reducable Outcome>
class ModelNode {
public:
    explicit ModelNode(Variable<Outcome> observations)
    : current_observations_(std::move(observations)) {
        using namespace util;
        auto n = current_observations_.total_count() + 1;
        HpFloat log_probability;
        if (current_observations_.dim() / n > 1e5) {
            log_probability = n * std::log(current_observations_.dim() / 2);
        } else if (auto d = current_observations_.long_dim(); d % 2 == 0) {
            log_probability = Gamma::lgammaInt(d / 2) - Gamma::lgammaInt(n + d / 2);
        } else {
            log_probability = Gamma::lgammaIntPlusHalf(d / 2) - Gamma::lgammaIntPlusHalf(n + d / 2);
        }

        for (const auto& [outcome, count]: current_observations_.counts()) {
            log_probability += Gamma::lgammaIntPlusHalf(count) - Gamma::lgammaIntPlusHalf(0) -
                    HpFloat(outcome.group_size()).log() * count;
        }

        base_measure_ = log_probability.exp();
    }

    util::HpFloat getProbabilityAfter(const Outcome& observation) const {
        return base_measure_ *
               (current_observations_.getCount(observation) + 0.5) / observation.group_size() / weight_;
    }

    [[nodiscard]]
    util::HpFloat getProbability() const {
        return base_measure_ *
            (current_observations_.total_count() + current_observations_.dim() / 2) / weight_;
    }

    util::HpFloat getSubTreeMeasureAfter(const Outcome& observation) const {
        util::HpFloat measure = base_measure_ *
                                (current_observations_.getCount(observation) + 0.5) / observation.group_size();;
        const std::vector<Outcome> reduced_observation = observation.computeReductions();
        assert(children_.size() == reduced_observation.size());
        for (int i = 0; i < children_.size(); ++i) {
            measure += children_[i].getSubTreeMeasureAfter(reduced_observation[i]);
        }
        return measure;
    }

    int createSubTree() {
        int node_count = 1;
        for (auto&& reduced: current_observations_.getReducedVariables()) {
            node_count += children_.emplace_back(std::move(reduced)).createSubTree();
        }
        return node_count;
    }

    void update_weight(const util::HpFloat& weight) {
        base_measure_ = base_measure_ * weight / weight_;
        weight_ = weight;
    }

    void updateObservations(const Variable<Outcome>& delta) {
        using namespace util;


        auto reduced_deltas = delta.getReducedVariables();
        for (int i = 0; i < children_.size(); ++i) {
            children_[i].updateObservations(reduced_deltas[i]);
        }

        const auto old_n = current_observations_.total_count() + 1;
        int n_diff = 0;
        HpFloat measure_diff = 0.0;
        for (const auto& [outcome, diff]: delta.counts()) {
            const auto old_count = current_observations_.add(outcome, diff);
            measure_diff += Gamma::lgammaIntPlusHalf(old_count + diff) - Gamma::lgammaIntPlusHalf(old_count) -
                    HpFloat(outcome.group_size()).log() * diff;
            n_diff += diff;
        }

        if (n_diff != 0) {
            const auto d = current_observations_.long_dim();
            if (d % 2 == 0) {
                measure_diff -=
                        Gamma::lgammaInt(old_n + n_diff + d / 2) - Gamma::lgammaInt(old_n + d / 2);
            } else {
                measure_diff -=
                        Gamma::lgammaIntPlusHalf(old_n + n_diff + d / 2) - Gamma::lgammaIntPlusHalf(old_n + d / 2);
            }
        }

        base_measure_ *= measure_diff.exp();
    }

    friend std::ostream& operator<<(std::ostream& os, const ModelNode& obj) {
        os << obj.current_observations_ << ":" << obj.getProbability() << " {";
        for (const auto& child: obj.children_) os << child << "  ";
        return os << "}";
    }

private:
    Variable<Outcome> current_observations_;
    std::vector<ModelNode> children_;
    util::HpFloat base_measure_;
    util::HpFloat weight_ = 1.0;
};
} // cprior::multinomial

#endif // INFERENCE_NODE_H
