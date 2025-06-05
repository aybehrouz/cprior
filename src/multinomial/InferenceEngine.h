//
// Created by aybehrouz on 6/3/25.
//

#ifndef CPRIOR_INFERENCE_ENGINE_H
#define CPRIOR_INFERENCE_ENGINE_H
#include <ostream>

#include "InferenceNode.h"
#include "Variable.h"

namespace cprior::multinomial {
template<Reducable Outcome>
class InferenceEngine {
public:
    InferenceEngine& addEvidence(Outcome&& observation, int count = 1) {
        if (root_ != nullptr) throw std::logic_error("Inference engine is closed");
        evidence_.add(std::move(observation), count);
        return *this;
    }

    InferenceEngine& processEvidence() {
        if (root_ != nullptr) throw std::logic_error("evidence already processed");
        root_ = std::make_unique<InferenceNode<Outcome> >(std::move(evidence_));
        size_ = root_->createSubTree();
        return *this;
    }

    std::vector<util::HpFloat> computeProbability(const std::vector<Outcome>& query) const {
        if (root_ == nullptr) throw std::logic_error("evidence not processed");
        std::vector<util::HpFloat> result;
        result.reserve(query.size());
        util::HpFloat sum = 0.0;
        for (auto&& outcome: query) {
            sum += result.emplace_back(root_->getSubTreeMeasureAfter(outcome));
        }
        if (result.size() > 1) for (auto& p: result) p /= sum;

        return result;
    }

    //todo: maybe unnecessary
    void update(Variable<Outcome> delta) {
        root_->updateObservations(delta);
    }

    friend std::ostream& operator<<(std::ostream& os, const InferenceEngine& obj) {
        return os << " size_: " << obj.size_ << " root_: " << *obj.root_;
    }

private:
    Variable<Outcome> evidence_{};
    std::unique_ptr<InferenceNode<Outcome> > root_{};
    std::size_t size_ = 0;
};
} // cprior::multinomial

#endif //CPRIOR_INFERENCE_ENGINE_H
