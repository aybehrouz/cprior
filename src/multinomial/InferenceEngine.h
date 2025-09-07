//
// Created by aybehrouz on 6/3/25.
//

#ifndef CPRIOR_INFERENCE_ENGINE_H
#define CPRIOR_INFERENCE_ENGINE_H
#include <ostream>

#include "Deterministic.h"
#include "ModelNode.h"
#include "Variable.h"
#include "encoder/Tuple.h"

namespace cprior::multinomial {
template<Reducable Outcome, class Model = ModelNode<Outcome>>
class InferenceEngine {
public:
    void AddEvidence(Outcome&& observation, int count = 1) {
        if constexpr (std::is_same_v<Outcome, encoder::Tuple::Instance>) {
            if (target_cardinality_ == 0) target_cardinality_ = observation.target_cardinality();
        }
        evidence_.Add(std::move(observation), count);
    }

    void ProcessEvidence() {
        if (root_ != nullptr) throw std::logic_error("Evidence already processed");

        if constexpr (std::is_same_v<Model, ModelNode<Outcome>>) {
            root_ = std::make_unique<Model>(std::move(evidence_));
            size_ = root_->CreateSubTree();
        } else {
            root_ = std::make_unique<Model>();
            for (const auto& [outcome, count]: evidence_.counts()) {
                root_->Add(outcome, count);
            }
            size_ = root_->CreateSubTree();
        }
    }

    const auto& MostProbableModel() const {
        return *root_->MostProbableChild();
    }

    int MostProbableOutcome(const std::vector<Outcome>& query) const {
        if (root_ == nullptr) throw std::logic_error("Evidence not processed");
        int result = 0;
        util::HpFloat max = 0.0;
        for (int i = 0; i < query.size(); ++i) {
            if (auto p = root_->TreeWeightedSumAfter(query[i]); p > max) {
                max = p;
                result = i;
            }
        }
        return result;
    }

    std::vector<util::HpFloat> ComputePosterior(const std::vector<Outcome>& query) const {
        if (root_ == nullptr) throw std::logic_error("Evidence not processed");
        std::vector<util::HpFloat> result;
        result.reserve(query.size());
        util::HpFloat sum = 0.0;
        for (auto&& outcome: query) {
            sum += result.emplace_back(root_->TreeWeightedSumAfter(outcome));
        }
        if (result.size() > 1) for (auto& p: result) p /= sum;

        return result;
    }

    friend std::ostream& operator<<(std::ostream& os, const InferenceEngine& obj) {
        return os << " size_: " << obj.size_ << " root_: " << *obj.root_;
    }

private:
    Variable<Outcome> evidence_{};
    std::unique_ptr<Model> root_{};
    std::size_t size_ = 0;
    int target_cardinality_ = 0;
};
} // cprior::multinomial

#endif //CPRIOR_INFERENCE_ENGINE_H
