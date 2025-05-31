//
// Created by aybehrouz on 5/21/25.
//

#include "Variable.h"


namespace cprior::multinomial {

std::ostream& operator<<(std::ostream& os, const Variable& obj) {
    os << "[";
    for (const auto& [outcome, count]: obj.counts_) {
        os << outcome << " = " << count << " ";
    }
    return os << "]";
}

} // cprior::multinomial
