
#ifndef CPRIOR_MUL_CENTRAL_PREDICTIVE_H
#define CPRIOR_MUL_CENTRAL_PREDICTIVE_H


#include "Variable.h"
#include "util/Gamma.h"
#include "util/HpFloat.h"

namespace cprior::multinomial  {

template<Reducable Outcome>
class CentralPredictive {
public:
    explicit CentralPredictive(Variable<Outcome> observations)
        : observations_(std::move(observations)) {
        using namespace util;

        auto n = observations_.total_count();
        auto d = observations_.dim();
        HpFloat log_probability = std::lgamma(d / 2) - std::lgamma(n + 1 + d / 2);

        for (const auto& [outcome, count]: observations_.counts()) {
            log_probability += Gamma::lgammaIntPlusHalf(count) - Gamma::lgammaIntPlusHalf(0) -
                    HpFloat(outcome.preimage_size()).log() * count;
        }

        measure_ = log_probability.exp();
    }

    util::HpFloat GetProbabilityAfter(const Outcome& observation) const {
        return measure_ * (observations_.count(observation) + 0.5) / observation.group_size();
    }

    [[nodiscard]]
    util::HpFloat probability() const {
        auto n = observations_.total_count();
        auto d = observations_.dim();
        return measure_ * (n + 1 + d / 2);
    }
private:
    Variable<Outcome> observations_;
    util::HpFloat measure_;
};

} // cprior::multinomial

#endif // CPRIOR_MUL_CENTRAL_PREDICTIVE_H