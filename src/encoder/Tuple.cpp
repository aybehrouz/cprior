//
// Created by aybehrouz on 6/13/25.
//

#include "Tuple.h"

#include "util/Gamma.h"

namespace cprior::encoder {
using std::vector, std::move, std::stringstream;

Tuple::Instance::Instance(const Tuple& tuple, const std::vector<std::string>& tokens)
    : tuple_(tuple),
      content_bit_blocks_(tuple.block_count(), 0),
      dim_(tuple.dim()),
      attr_count_(tuple.attribute_count()) {
    if (!tuple.closed_) throw std::runtime_error("Tuple is not closed");
    if (tokens.size() != tuple_.entry_count()) {
        throw std::invalid_argument("Wrong number of tokens: " + std::to_string(tokens.size()));
    }
    for (int i = 0; i < tuple_.entry_count(); ++i) {
        SetValueOf_(tuple.entry(i), tokens[i]);
    }
}

vector<Tuple::Instance> Tuple::Instance::ComputeReductions() const {
    if (attr_count_ <= min_attributes_) return {};

    vector<Instance> result;
    for (int i = head_attr_; i < tuple_.attribute_count(); ++i) {
        auto reduced = *this;
        reduced.RemoveAttribute_(i);
        reduced.head_attr_ = i + 1;
        if (attr_count_ > max_attributes_ + 1) {
            for (auto && next_reductions: reduced.ComputeReductions()) {
                result.emplace_back(std::move(next_reductions));
            }
        } else {
            result.emplace_back(std::move(reduced));
        }
    }
    return result;
}

std::pair<std::vector<Tuple::Instance>, Entry::IntType> Tuple::Instance::ComputeTargetInstances() const {
    const auto& target = tuple_.entry(tuple_.index_of_target_);
    const auto target_value = IntValueOf_(target);

    std::vector result(target.cardinality(), *this);
    for (Entry::IntType i = 1; i <= target.cardinality(); ++i) {
        result[i - 1].SetValueOf_(target, i);
    }
    return {result, target_value - 1};
}

std::string Tuple::Instance::attribute_str() const {
    if (attribute_count() == 0) return {};
    stringstream ss;
    ss << attribute_str(0);
    for (int i = 1; i < this->attribute_count(); ++i) ss << "," << attribute_str(i);
    return ss.str();
}

void Tuple::Instance::RemoveAttribute_(int index) {
    const auto& attr = tuple_.attribute(index);
    content_bit_blocks_[attr.block_index()] &= attr.negative_mask();
    group_size_ *= attr.cardinality();
    dim_ /= attr.cardinality();
    --attr_count_;
}

Tuple& Tuple::AddEntry(std::unique_ptr<Entry> e) {
    if (closed_) throw std::logic_error("AddEntry called on a closed tuple");
    auto& entry = *entries_.emplace_back(std::move(e));
    if (current_offset_ + entry.bit_len() <= kBlockSize) {
        entry.set_position(current_block_, current_offset_);
        current_offset_ += entry.bit_len();
    } else {
        ++current_block_;
        entry.set_position(current_block_, 0);
        current_offset_ = entry.bit_len();
    }
    dim_ *= entry.cardinality();
    return *this;
}

std::string Tuple::to_string() const {
    stringstream ss;
    ss << index_of_target_ << std::endl;
    for (const auto& entry: entries_) ss << entry->to_string() << std::endl;
    return ss.str();
}

std::ostream& operator<<(std::ostream& os, const Tuple::Instance& obj) {
    os << "(" << obj.attribute_str() << ") -> <" << obj.target_str() << "> ";
    os << "dim:" << obj.dim() << " g:" << obj.group_size() << " ##";
    for (const auto attr: obj.content_bit_blocks_) os << std::bitset<64>(attr) << "|";
    return os;
}
}
