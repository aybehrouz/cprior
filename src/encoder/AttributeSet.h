//
// Created by aybehrouz on 6/1/25.
//

#ifndef CPRIOR_ATTRIBUTE_SET_H
#define CPRIOR_ATTRIBUTE_SET_H

#include <ostream>
#include <utility>

namespace cprior::encoder {
class AttributeType {
public:
    explicit AttributeType(std::string name, std::vector<std::string> categories)
        : name_(std::move(name)) {
        for (auto& category: categories) {
            auto [new_pair, inserted] =
                    value_map_.try_emplace(std::move(category), value_map_.size() + 1);
            if (!inserted) throw std::invalid_argument("category already exists");
            category_map_.emplace_back(new_pair->first);
        }
        bit_len_ = 8 * sizeof(value_map_.size()) - std::countl_zero(value_map_.size());
    }

    void set_position(unsigned block_index, unsigned block_offset) {
        negative_mask_ = ~(((1 << bit_len_) - 1) << block_offset);
        block_index_ = block_index;
        block_offset_ = block_offset;
    }

    unsigned int intValueFromString(const std::string& text) const {
        return value_map_.at(text);
    }

    std::string_view stringFromIntValue(unsigned value) const {
        return category_map_[value];
    }

    [[nodiscard]]
    unsigned long negative_mask() const {
        return negative_mask_;
    }

    unsigned int bit_len() const {
        return bit_len_;
    }

    unsigned int cardinality() const {
        return value_map_.size();
    }

    [[nodiscard]] unsigned block_index() const {
        return block_index_;
    }

    [[nodiscard]] unsigned block_offset() const {
        return block_offset_;
    }

    friend std::ostream& operator<<(std::ostream& os, const AttributeType& obj) {
        os << "att: " << obj.name_ << "[";
        for (const auto& [k, v]: obj.value_map_) {
            os << k << ":" << v << ", ";
        }
        os << "]";
        return os
               << " negative_mask_: " << std::bitset<64>(obj.negative_mask_)
               << " block_index_: " << obj.block_index_
               << " block_offset_: " << obj.block_offset_
               << " bit_len_: " << obj.bit_len_;
    }

private:
    std::unordered_map<std::string, unsigned int> value_map_;
    std::vector<std::string_view> category_map_ = {"<Removed>"};
    std::string name_;
    unsigned long negative_mask_ = -1;
    unsigned block_index_ = 0;
    unsigned block_offset_ = 0;
    unsigned bit_len_;
};

class AttributeSet {
public:
    using BlockType = unsigned long;
    static constexpr unsigned kBlockSize = 8 * sizeof(BlockType);

    int attr_count() const {
        return entry_types_.size() - 1;
    }

    int entry_count() const {
        return entry_types_.size();
    }

    double dim() const {
        return dim_;
    }

    const AttributeSet& addAttribute(AttributeType* attribute) {
        entry_types_.emplace_back(attribute);
        if (current_offset_ + attribute->bit_len() <= kBlockSize) {
            attribute->set_position(current_block_, current_offset_);
            current_offset_ += attribute->bit_len();
        } else {
            ++current_block_;
            attribute->set_position(current_block_, 0);
            current_offset_ = attribute->bit_len();
        }
        dim_ *= attribute->cardinality();
        return *this;
    }

    [[nodiscard]]
    const AttributeType& getEntryType(int entry_index) const {
        return *entry_types_[entry_index];
    }

    [[nodiscard]]
   const AttributeType& getAttributeType(int attribute_index) const {
        attribute_index = attribute_index >= target_idx_ ? attribute_index + 1 : attribute_index;
        return *entry_types_[attribute_index];
    }

    [[nodiscard]]
    unsigned block_count() const {
        return current_block_ + 1;
    }

private:
    std::vector<AttributeType*> entry_types_;
    int target_idx_;
    unsigned current_block_ = 0;
    unsigned current_offset_ = 0;
    double dim_ = 1;
};
} // cprior::encoder

#endif //CPRIOR_ATTRIBUTE_SET_H
