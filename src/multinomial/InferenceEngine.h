//
// Created by aybehrouz on 6/3/25.
//

#ifndef CPRIOR_INFERENCE_ENGINE_H
#define CPRIOR_INFERENCE_ENGINE_H
#include <ostream>

#include "ModelNode.h"
#include "Variable.h"

namespace cprior::multinomial {
template<Reducable Outcome>
class InferenceEngine {
public:
    void AddEvidence(Outcome&& observation, int count = 1) {
        evidence_.add(std::move(observation), count);
    }

    InferenceEngine& ProcessEvidence() {
        if (root_ != nullptr) throw std::logic_error("Evidence already processed");
        root_ = std::make_unique<ModelNode<Outcome>>(std::move(evidence_));
        size_ = root_->createSubTree();
        return *this;
    }

    int MostProbableOutcome(const std::vector<Outcome>& query) const {
        int result = 0;
        util::HpFloat max = 0.0;
        for (int i = 0; i < query.size(); ++i) {
            if (auto p = root_->getTreeWeightedSumAfter(query[i]); p > max) {
                max = p;
                result = i;
            }
        }
        return result;
    }

    std::vector<util::HpFloat> computePosterior(const std::vector<Outcome>& query) const {
        if (root_ == nullptr) throw std::logic_error("Evidence not processed");
        std::vector<util::HpFloat> result;
        result.reserve(query.size());
        util::HpFloat sum = 0.0;
        for (auto&& outcome: query) {
            sum += result.emplace_back(root_->getTreeWeightedSumAfter(outcome));
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
    std::unique_ptr<ModelNode<Outcome>> root_{};
    std::size_t size_ = 0;
};
} // cprior::multinomial

#endif //CPRIOR_INFERENCE_ENGINE_H
