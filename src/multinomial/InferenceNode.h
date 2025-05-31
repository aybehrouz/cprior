//
// Created by aybehrouz on 5/23/25.
//

#ifndef INFERENCE_NODE_H
#define INFERENCE_NODE_H
#include <ostream>
#include <vector>

#include "Variable.h"
#include "../math/HpFloat.h"


namespace cprior::multinomial {
class InferenceNode {
public:
    explicit InferenceNode(Variable current_observations);

    math::HpFloat getProbabilityAfter(const Outcome& observation) const {
        return base_measure_ *
            (current_observations_.getCount(observation) + 0.5) / observation.group_size() / weight_;
    }

    math::HpFloat getSubTreeMeasureAfter(const Outcome& observation) const;

    void createSubTree();

    void set_weight(const math::HpFloat& weight) {
        base_measure_ = base_measure_ * weight / weight_;
        weight_ = weight;
    }

    friend std::ostream& operator<<(std::ostream& os, const InferenceNode& obj);

private:
    Variable current_observations_;
    std::vector<InferenceNode> children_;
    math::HpFloat base_measure_;
    math::HpFloat weight_ = 1.0;
};
} // cprior::multinomial

#endif // INFERENCE_NODE_H
