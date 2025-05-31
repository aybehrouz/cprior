//
// Created by aybehrouz on 5/23/25.
//

#ifndef ATTRIBUTE_TUPLE_H
#define ATTRIBUTE_TUPLE_H
#include <ostream>
#include <vector>

#include "../math/HpFloat.h"


namespace cprior::multinomial {
class AttributeTuple {
public:
    AttributeTuple(std::vector<int> attributes, bool target)
        : attributes_(std::move(attributes)),
          target_(target) {
    }

    [[nodiscard]]
    int num_of_reductions() const;

    math::HpFloat group_size() const {
        if (attributes_.size() <= 1) return 4;
        return 1;
    }

    double dim() const {
        return 1 << (attributes_.size() + 1);
    }

    [[nodiscard]]
    std::size_t hash() const {
        return target_ * 12;
    }

    [[nodiscard]]
    std::vector<AttributeTuple> computeReductions() const;

    friend std::ostream& operator<<(std::ostream& os, const AttributeTuple& obj);

    friend bool operator==(const AttributeTuple& lhs, const AttributeTuple& rhs) {
        return lhs.attributes_ == rhs.attributes_
               && lhs.target_ == rhs.target_;
    }

    friend bool operator!=(const AttributeTuple& lhs, const AttributeTuple& rhs) {
        return !(lhs == rhs);
    }


private:
    std::vector<int> attributes_;
    bool target_;
};

using Outcome = AttributeTuple;

} //  cprior::multinomial

template<>
struct std::hash<cprior::multinomial::AttributeTuple>
{
    std::size_t operator()(const cprior::multinomial::AttributeTuple& tuple) const noexcept {
        return tuple.hash();
    }
};

#endif //ATTRIBUTE_TUPLE_H
