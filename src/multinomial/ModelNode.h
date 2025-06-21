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

    util::HpFloat NodeProbabilityAfter(const Outcome& observation) const {
        return base_measure_ *
               (current_observations_.count(observation) + 0.5) / observation.group_size() / weight_;
    }

    [[nodiscard]]
    util::HpFloat NodeProbability() const {
        return measure() / weight_;
    }

    [[nodiscard]]
    util::HpFloat measure() const {
        return base_measure_ *
               (current_observations_.total_count() + current_observations_.dim() / 2);
    }

    [[nodiscard]]
    util::HpFloat TreeWeightedSum() const {
        auto m = measure();
        for (const auto& child: children_) {
            m += child.TreeWeightedSum();
        }
        return m;
    }

    const ModelNode* MostProbableChild() const {
        auto m = measure();
        const auto* result = this;
        for (const auto& child: children_) {
            const auto* sub_tree_best = child.MostProbableChild();
            if (m < sub_tree_best->measure()) {
                m = sub_tree_best->measure();
                result = sub_tree_best;
            }
        }
        return result;
    }

    util::HpFloat TreeWeightedSumAfter(const Outcome& observation) const {
        util::HpFloat measure = base_measure_ *
                                (current_observations_.count(observation) + 0.5) / observation.group_size();;
        const std::vector<Outcome> reduced_observation = observation.ComputeReductions();
        assert(children_.size() == reduced_observation.size());
        for (int i = 0; i < children_.size(); ++i) {
            measure += children_[i].TreeWeightedSumAfter(reduced_observation[i]);
        }
        return measure;
    }

    int CreateSubTree() {
        int node_count = 1;
        for (auto&& reduced: current_observations_.ComputeReducedVariables()) {
            node_count += children_.emplace_back(std::move(reduced)).CreateSubTree();
        }
        return node_count;
    }

    void set_weight(const util::HpFloat& weight) {
        base_measure_ = base_measure_ * weight / weight_;
        weight_ = weight;
    }

    void UpdateObservations(const Variable<Outcome>& delta) {
        if (delta.empty()) return;
        using namespace util;

        auto reduced_deltas = delta.ComputeReducedVariables();
        for (int i = 0; i < children_.size(); ++i) {
            children_[i].UpdateObservations(reduced_deltas[i]);
        }

        const auto old_n = current_observations_.total_count() + 1;
        int n_diff = 0;
        HpFloat measure_diff = 0.0;
        for (const auto& [outcome, diff]: delta.counts()) {
            const auto old_count = current_observations_.Add(outcome, diff);
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
        os << obj.measure() << ":" << obj.current_observations_ << " {";
        for (const auto& child: obj.children_) os << child << "  ";
        return os << "}";
    }

    static ModelNode CreateModelTree(int n, int iterations) {
        std::vector<Outcome> outcomes = Outcome::possibleOutcomes();
        util::ConstantSumRange possible_counts(n, n, outcomes.size());
        Variable<Outcome> init;
        init.Add(outcomes.back(), n);
        ModelNode root(std::move(init));
        root.CreateSubTree();
        auto previous = *possible_counts.begin();
        for (int iteration = 1; iteration <= iterations; ++iteration) {
            for (const auto& current: possible_counts) {
                Variable<Outcome> delta;
                for (int i = 0; i < current.size(); ++i) {
                    if (int diff = current[i] - previous[i]; diff != 0) {
                        delta.Add(outcomes[i], diff);
                    }
                }
                root.UpdateObservations(delta);
                UpdateLogPosterior_(root, root.TreeWeightedSum(), util::Gamma::MultiChoose(current));
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
        auto c = root.NodeProbability();
        if (c.to_double() != 0) {
            root.expected_log_posterior_ += coefficient * c * (c.log() + root.weight_.log() - weighted_sum.log());
        }
        for (auto& child: root.children_) UpdateLogPosterior_(child, weighted_sum, coefficient);
    }

    static void UpdateWeights_(ModelNode& root) {
        root.set_weight(root.expected_log_posterior_.exp());
        std::cout << root.weight_.to_double() << " ";
        root.expected_log_posterior_ = 0.0;
        for (auto& child: root.children_) UpdateWeights_(child);
    }
};
} // cprior::multinomial

#endif // INFERENCE_NODE_H
