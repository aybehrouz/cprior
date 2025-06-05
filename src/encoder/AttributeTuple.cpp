//
// Created by aybehrouz on 5/23/25.
//

#include "AttributeTuple.h"

#include <iterator>

namespace cprior::encoder {
using namespace std;

AttributeTuple::AttributeTuple(const std::string& value_text, const AttributeSet& attr_types)
    : content_bit_blocks_(attr_types.block_count(), 0),
      dim_(attr_types.dim()),
      attr_count_(attr_types.attr_count()),
      type_manager_(attr_types) {
    const std::regex reg("(\\s|,)+");
    auto token_iter = std::sregex_token_iterator(value_text.begin(), value_text.end(), reg, -1);
    for (int i = 0; i < type_manager_.attr_count(); ++i) {
        if (token_iter == std::sregex_token_iterator()) throw std::invalid_argument("missing some attributes");
        const auto& type = type_manager_.getEntryType(i);
        content_bit_blocks_[type.block_index()] |=
                type.intValueFromString(token_iter->str()) << type.block_offset();
        ++token_iter;
    }
}

vector<AttributeTuple> AttributeTuple::computeReductions() const {
    vector<AttributeTuple> result;
    for (int i = head_idx_; i < type_manager_.attr_count(); ++i) {
        auto& inserted = result.emplace_back(*this);
        inserted.removeAttribute_(i);
        inserted.head_idx_ = i + 1;
    }
    return result;
}

ostream& operator<<(ostream& os, const AttributeTuple& obj) {
    os << "<";
    for (const auto attr: obj.content_bit_blocks_) os << bitset<64>(attr) << "|";
    os << "d:" << obj.dim_;
    return os;
}
}
