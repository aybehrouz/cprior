//
// Created by aybehrouz on 5/23/25.
//

#ifndef ATTRIBUTE_TUPLE_H
#define ATTRIBUTE_TUPLE_H
#include <regex>
#include <vector>

#include "AttributeSet.h"
#include "../util/HpFloat.h"


namespace cprior::encoder {
class AttributeTuple {
public:
    AttributeTuple(const std::string& value_text, const AttributeSet& attr_types);

    [[nodiscard]]
    int num_of_reductions() const {
        return 1 << attr_count_;
    }

    util::HpFloat group_size() const {
        return group_size_;
    }

    [[nodiscard]]
    double dim() const {
        return dim_;
    }

    void set_attribute_value(int attr_index, unsigned int value) {
    }

    [[nodiscard]]
    std::size_t hash() const {
        std::size_t seed = 0x651A07A9;
        for (auto bit_block: content_bit_blocks_) {
            seed ^= (seed << 6) + (seed >> 2) + 0x27AE1971 + bit_block;
        }
        return seed;
    }

    [[nodiscard]]
    std::vector<AttributeTuple> computeReductions() const;

    friend std::ostream& operator<<(std::ostream& os, const AttributeTuple& obj);

    friend bool operator==(const AttributeTuple& lhs, const AttributeTuple& rhs) {
        return lhs.content_bit_blocks_ == rhs.content_bit_blocks_;
    }

    friend bool operator!=(const AttributeTuple& lhs, const AttributeTuple& rhs) {
        return !(lhs == rhs);
    }

private:
    std::vector<AttributeSet::BlockType> content_bit_blocks_{0};
    int head_idx_ = 0;
    const AttributeSet& type_manager_;
    double group_size_ = 1;
    double dim_;
    int attr_count_;

    void removeAttribute_(int attr_index) {
        const auto& attr = type_manager_.getAttributeType(attr_index);
        content_bit_blocks_[attr.block_index()] &= attr.negative_mask();
        group_size_ *= attr.cardinality();
        dim_ /= attr.cardinality();
        --attr_count_;
    }
};
} //  cprior::encoder

template<>
struct std::hash<cprior::encoder::AttributeTuple> {
    std::size_t operator()(const cprior::encoder::AttributeTuple& tuple) const noexcept {
        return tuple.hash();
    }
};

#endif //ATTRIBUTE_TUPLE_H
