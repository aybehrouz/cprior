//
// Created by aybehrouz on 5/23/25.
//

#include "AttributeTuple.h"

#include <iterator>

namespace cprior::multinomial {
using namespace std;

int AttributeTuple::num_of_reductions() const {
    if (attributes_.size() <= 1) return 0;
    return static_cast<int>(attributes_.size());
}

vector<AttributeTuple> AttributeTuple::computeReductions() const {
    if (num_of_reductions() == 0) return {};
    vector<AttributeTuple> result;
    result.reserve(num_of_reductions());
    for (auto attribute: attributes_) {
        result.push_back(AttributeTuple({attribute}, target_));
    }
    return result;
}

ostream& operator<<(ostream& os, const AttributeTuple& obj) {
    os << "<";
    for (const auto attribute: obj.attributes_) os << attribute << ", ";
    os << "=> " << obj.target_ << ">";
    return os;
}
}
