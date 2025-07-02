//
// Created by aybehrouz on 6/13/25.
//

#ifndef CPRIOR_ATTRIBUTE_H
#define CPRIOR_ATTRIBUTE_H
#include <bit>
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
    virtual std::string to_string() const = 0;

    [[nodiscard]]
    virtual IntType IntValueFromString(const std::string& text) const = 0;

    [[nodiscard]]
    virtual std::string StringFromIntValue(IntType value) const = 0;

    void set_position(unsigned block_index, unsigned block_offset);

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

    friend std::ostream& operator<<(std::ostream& os, const Entry& obj);

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
    explicit NominalEntry(std::string name, std::vector<std::string> categories);

    std::string to_string() const override;

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
    OrdinalEntry(std::string name, unsigned cardinality, double min_value, double max_value)
        : Entry(std::move(name), cardinality),
          min_value_(min_value),
          max_value_(max_value) {
    }

    [[nodiscard]]
    std::string to_string() const override;

    [[nodiscard]]
    IntType IntValueFromString(const std::string& text) const override;

    [[nodiscard]]
    std::string StringFromIntValue(IntType value) const override;

private:
    double min_value_;
    double max_value_;
};

} // namespace cprior::encoder

#endif // CPRIOR_ATTRIBUTE_H
