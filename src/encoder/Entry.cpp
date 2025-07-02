//
// Created by aybehrouz on 6/13/25.
//

#include "Entry.h"

#include <cmath>
#include <ranges>

namespace cprior::encoder {

void Entry::set_position(unsigned block_index, unsigned block_offset) {
    negative_mask_ = ~(((static_cast<IntType>(1) << bit_len_) - 1) << block_offset);
    block_index_ = block_index;
    block_offset_ = block_offset;
}

NominalEntry::NominalEntry(std::string name, std::vector<std::string> categories)
    : Entry(std::move(name), categories.size()) {
    for (auto& category: categories) {
        auto [new_pair, inserted] =
                value_map_.try_emplace(std::move(category), value_map_.size() + 1);
        if (!inserted) throw std::invalid_argument("category already exists");
        category_map_.emplace_back(new_pair->first);
    }
}

std::string NominalEntry::to_string() const {
    std::stringstream ss;
    ss << "Nominal " << name_ << ",";
    for (const auto& cat: value_map_ | std::views::keys) ss << cat << ",";
    return ss.str();
}

std::string OrdinalEntry::to_string() const {
    std::stringstream ss;
    ss << "Ordinal " << name_ << "," << cardinality_ << "," << min_value_ << "," << max_value_;
    return ss.str();
}

Entry::IntType OrdinalEntry::IntValueFromString(const std::string& text) const {
    const auto ratio = (std::stod(text) - min_value_) / (max_value_ - min_value_);
    return static_cast<IntType>(std::round(ratio * (cardinality_ - 1))) + 1;
}

std::string OrdinalEntry::StringFromIntValue(IntType value) const {
    if (value == 0) return "<Nil>";
    const auto ratio = static_cast<double>(value - 1) / (cardinality_ - 1);
    return std::to_string(ratio * (max_value_ - min_value_) + min_value_);
}

std::ostream& operator<<(std::ostream& os, const Entry& obj) {
    return os
           << "name_: " << obj.name_
           << " cardinality_: " << obj.cardinality_
           << " bit_len_: " << obj.bit_len_
           << " negative_mask_: " << std::bitset<64>(obj.negative_mask_)
           << " block_index_: " << obj.block_index_
           << " block_offset_: " << obj.block_offset_;
}
}
