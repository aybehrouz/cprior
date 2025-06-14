//
// Created by aybehrouz on 6/13/25.
//

#ifndef CPRIOR_ATTRIBUTE_H
#define CPRIOR_ATTRIBUTE_H
#include <bit>
#include <cmath>
#include <iostream>
#include <ostream>
#include <regex>
#include <string>

namespace cprior::encoder {

class Entry {
public:
    using IntType = unsigned long;
    virtual ~Entry() = default;

    explicit Entry(std::string name, unsigned cardinality)
        : name_(std::move(name)),
          cardinality_(cardinality),
          bit_len_(8 * sizeof(cardinality) - std::countl_zero(cardinality)) {
    }

    [[nodiscard]]
    virtual IntType IntValueFromString(const std::string& text) const = 0;

    [[nodiscard]]
    virtual std::string StringFromIntValue(IntType value) const = 0;

    void set_position(unsigned block_index, unsigned block_offset) {
        negative_mask_ = ~(((static_cast<IntType>(1) << bit_len_) - 1) << block_offset);
        block_index_ = block_index;
        block_offset_ = block_offset;
    }

    [[nodiscard]] unsigned cardinality() const {
        return cardinality_;
    }

    [[nodiscard]] unsigned bit_len() const {
        return bit_len_;
    }

    [[nodiscard]] IntType negative_mask() const {
        return negative_mask_;
    }

    [[nodiscard]] unsigned block_index() const {
        return block_index_;
    }

    [[nodiscard]] unsigned block_offset() const {
        return block_offset_;
    }

    friend std::ostream& operator<<(std::ostream& os, const Entry& obj) {
        return os
               << "name_: " << obj.name_
               << " cardinality_: " << obj.cardinality_
               << " bit_len_: " << obj.bit_len_
               << " negative_mask_: " << std::bitset<64>(obj.negative_mask_)
               << " block_index_: " << obj.block_index_
               << " block_offset_: " << obj.block_offset_;
    }

protected:
    std::string name_;
    unsigned cardinality_;
    unsigned bit_len_;
    IntType negative_mask_ = -1;
    unsigned block_index_ = 0;
    unsigned block_offset_ = 0;
};

class NominalEntry : public Entry {
public:
    explicit NominalEntry(std::vector<std::string> tokens)
       : Entry(std::move(tokens.at(0)), tokens.size() - 1) {
       for (int i = 1; i < tokens.size(); ++i) {
           auto [new_pair, inserted] =
                    value_map_.try_emplace(std::move(tokens[i]), value_map_.size() + 1);
           if (!inserted) throw std::invalid_argument("category already exists");
           category_map_.emplace_back(new_pair->first);
       }
    }

    IntType IntValueFromString(const std::string& text) const override {
        return value_map_.at(text);
    }

    std::string StringFromIntValue(IntType value) const override {
        return std::string(category_map_.at(value));
    }

private:
    std::unordered_map<std::string, IntType> value_map_;
    std::vector<std::string_view> category_map_ = {"<Nil>"};
};

class OrdinalEntry : public Entry {
public:
    explicit OrdinalEntry(std::vector<std::string> tokens)
        : Entry(std::move(tokens.at(0)), std::stoi(tokens.at(1))),
          min_value_(std::stod(tokens.at(2))), max_value_(std::stod(tokens.at(3))) {
    }

    [[nodiscard]]
    IntType IntValueFromString(const std::string& text) const override {
        const auto ratio = (std::stod(text) - min_value_) / (max_value_ - min_value_);
        return static_cast<IntType>(std::round(ratio * (cardinality_ - 1))) + 1;
    }

    [[nodiscard]]
    std::string StringFromIntValue(IntType value) const override {
        const auto ratio = static_cast<double>(value - 1) / (cardinality_ - 1);
        return std::to_string(ratio * (max_value_ - min_value_) + min_value_);
    }

private:
    double min_value_;
    double max_value_;
};

} // namespace cprior::encoder

#endif // CPRIOR_ATTRIBUTE_H
