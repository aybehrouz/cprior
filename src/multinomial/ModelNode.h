//
// Created by aybehrouz on 5/23/25.
//

#ifndef INFERENCE_NODE_H
#define INFERENCE_NODE_H
#include <ostream>
#include <vector>
#include <cassert>
#include <iostream>

#include "Variable.h"
#include "util/ConstantSumRange.h"
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

    util::HpFloat getNodeProbabilityAfter(const Outcome& observation) const {
        return base_measure_ *
               (current_observations_.getCount(observation) + 0.5) / observation.group_size() / weight_;
    }

    [[nodiscard]]
    util::HpFloat getNodeProbability() const {
        return base_measure_ *
            (current_observations_.total_count() + current_observations_.dim() / 2) / weight_;
    }

    [[nodiscard]]
    util::HpFloat getTreeWeightedSum() const {
        util::HpFloat measure = base_measure_ *
                                (current_observations_.total_count() + current_observations_.dim() / 2);
        for (const auto& child: children_) {
            measure += child.getTreeWeightedSum();
        }
        return measure;
    }

    util::HpFloat getTreeWeightedSumAfter(const Outcome& observation) const {
        util::HpFloat measure = base_measure_ *
                                (current_observations_.getCount(observation) + 0.5) / observation.group_size();;
        const std::vector<Outcome> reduced_observation = observation.ComputeReductions();
        assert(children_.size() == reduced_observation.size());
        for (int i = 0; i < children_.size(); ++i) {
            measure += children_[i].getTreeWeightedSumAfter(reduced_observation[i]);
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
        if (delta.empty()) return;
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
        os << obj.current_observations_ << ":" << obj.getNodeProbability() << " {";
        for (const auto& child: obj.children_) os << child << "  ";
        return os << "}";
    }

    static ModelNode CreateModelTree(int n, int iterations) {
        std::vector<Outcome> outcomes = Outcome::possibleOutcomes();
        util::ConstantSumRange possible_counts(n, n, outcomes.size());
        Variable<Outcome> init;
        init.add(outcomes.back(), n);
        ModelNode root(std::move(init));
        root.createSubTree();
        auto previous = *possible_counts.begin();
        for (int iteration = 1; iteration <= iterations; ++iteration) {
            for (const auto& current: possible_counts) {
                Variable<Outcome> delta;
                for (int i = 0; i < current.size(); ++i) {
                    if (int diff = current[i] - previous[i]; diff != 0) {
                        delta.add(outcomes[i], diff);
                    }
                }
                root.updateObservations(delta);
                UpdateLogPosterior_(root, root.getTreeWeightedSum(), util::Gamma::MultiChoose(current));
                previous = current;
            }
            std::cout << "iteration #" << iteration << ": ";
            UpdateWeights_(root);
            std::cout << std::endl;
        }

        return root;
    }

private:
    Variable<Outcome> current_observations_;
    std::vector<ModelNode> children_;
    util::HpFloat base_measure_ = 0.0;
    util::HpFloat weight_ = 1.0;
    util::HpFloat expected_log_posterior_ = 0.0;


    static void UpdateLogPosterior_(ModelNode& root,
                                  const util::HpFloat& weighted_sum, const util::HpFloat& coefficient) {
        auto c = root.getNodeProbability();
        if (c.to_double() != 0) {
            root.expected_log_posterior_ += coefficient * c * (c.log() + root.weight_.log() - weighted_sum.log());
        }
        for (auto& child: root.children_) UpdateLogPosterior_(child, weighted_sum, coefficient);
    }

    static void UpdateWeights_(ModelNode& root) {
        root.update_weight(root.expected_log_posterior_.exp());
        std::cout << root.weight_.to_double() << " ";
        root.expected_log_posterior_ = 0.0;
        for (auto& child: root.children_) UpdateWeights_(child);
    }
};
} // cprior::multinomial

#endif // INFERENCE_NODE_H
