//
// Created by aybehrouz on 5/23/25.
//

#include "InferenceNode.h"

#include <cassert>
#include <cmath>
#include <iostream>

#include "../math/Gamma.h"

using namespace cprior::math;


namespace cprior::multinomial {
InferenceNode::InferenceNode(Variable observations): current_observations_(std::move(observations)) {
    auto n = current_observations_.total_count() + 1;
    HpFloat log_probability;
    if (current_observations_.dim() / n > 1e5) {
        log_probability = n * std::log(current_observations_.dim() / 2);
    } else if (long d = std::lround(current_observations_.dim()); d % 2 == 0) {
        log_probability = Gamma::logInt(d / 2) - Gamma::logInt(n + d / 2);
    } else {
        log_probability = Gamma::logHalfInt(d / 2) - Gamma::logHalfInt(n + d / 2);
    }

    for (const auto& [outcome, count]: current_observations_.counts()) {
        log_probability += Gamma::logHalfInt(count) - Gamma::logHalfInt(0) -
                outcome.group_size().log() * count;
    }

    base_measure_ = weight_ * log_probability.exp();
}

HpFloat InferenceNode::getSubTreeMeasureAfter(const Outcome& observation) const {
    HpFloat measure = base_measure_ *
            (current_observations_.getCount(observation) + 0.5) / observation.group_size();;
    std::cout << "##--> " << measure << std::endl;
    const std::vector<Outcome> reduced_observation = observation.computeReductions();
    assert(children_.size() == reduced_observation.size());
    for (int i = 0; i < children_.size(); ++i) {
        measure += children_[i].getSubTreeMeasureAfter(reduced_observation[i]);
    }
    return measure;
}

void InferenceNode::createSubTree() {
    std::vector<Variable> reduced_observations(current_observations_.num_of_reductions());
    for (const auto& [outcome, count]: current_observations_.counts()) {
        std::vector<Outcome> reductions = outcome.computeReductions();
        assert(reductions.size() == reduced_observations.size());
        for (int i = 0; i < reduced_observations.size(); ++i) {
            reduced_observations[i].add(std::move(reductions[i]), count);
        }
    }

    for (auto&& ro: reduced_observations) {
        children_.emplace_back(std::move(ro)).createSubTree();
    }
}

std::ostream& operator<<(std::ostream& os, const InferenceNode& obj) {
    os << obj.current_observations_ << " {";
    for (const auto& child: obj.children_) os << child << "  ";
    return os << "}";
}
} // cprior::multinomial
