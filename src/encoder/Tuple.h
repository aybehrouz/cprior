//
// Created by aybehrouz on 6/1/25.
//

#ifndef CPRIOR_ENCODER_TUPLE_H
#define CPRIOR_ENCODER_TUPLE_H

#include <utility>
#include <limits>

#include "Entry.h"

namespace cprior::encoder {
class Tuple {
public:
    using BlockType = Entry::IntType;
    static constexpr unsigned kBlockSize = 8 * sizeof(BlockType);

    explicit Tuple(int index_of_target)
        : index_of_target_(index_of_target) {
    }

    [[nodiscard]]
    int attribute_count() const {
        return static_cast<int>(entries_.size() - 1);
    }

    [[nodiscard]]
    int entry_count() const {
        return static_cast<int>(entries_.size());
    }

    [[nodiscard]]
    double dim() const {
        return dim_;
    }

    void close() {
        closed_ = true;
    }

    Tuple& AddEntry(std::unique_ptr<Entry> e);

    Tuple& AddNominalEntry(std::vector<std::string> tokens) {
        return this->AddEntry(std::make_unique<NominalEntry>(std::move(tokens)));
    }

    Tuple& AddOrdinalEntry(std::vector<std::string> tokens) {
        return this->AddEntry(std::make_unique<OrdinalEntry>(std::move(tokens)));
    }

    [[nodiscard]]
    const Entry& entry(int index) const {
        return *entries_[index];
    }

    std::string to_string() const;

    [[nodiscard]]
    const Entry& attribute(int index) const {
        index = index >= index_of_target_ ? index + 1 : index;
        return *entries_[index];
    }

    [[nodiscard]]
    unsigned block_count() const {
        return current_block_ + 1;
    }

    static void ChangeMaxMinAttributes(int min, int max) {
        min_attribute_count_ = min;
        max_attribute_count_ = max;
    }

    class Instance {
    public:
        Instance(const Tuple& tuple, const std::vector<std::string>& tokens);

        std::vector<Instance> ComputeReductions() const;

        std::pair<std::vector<Instance>, Entry::IntType> ComputeTargetInstances() const;

        std::string attribute_str(int index) const {
            return StringValueOf_(tuple_.attribute(index));
        }

        std::string target_str() const {
            return StringValueOf_(tuple_.entry(tuple_.index_of_target_));
        }

        [[nodiscard]]
        int num_of_reductions() const;

        [[nodiscard]]
        double group_size() const {
            return group_size_;
        }

        [[nodiscard]]
        double dim() const {
            return dim_;
        }

        [[nodiscard]]
        std::size_t hash() const {
            if (content_bit_blocks_.size() == 1) return content_bit_blocks_.back();
            std::size_t hash = 0x651A07A9;
            for (const auto bit_block: content_bit_blocks_) {
                hash ^= (hash << 6) + (hash >> 2) + 0x27AE1971 + bit_block;
            }
            return hash;
        }

        friend bool operator==(const Instance& lhs, const Instance& rhs) {
            return lhs.content_bit_blocks_ == rhs.content_bit_blocks_;
        }

        friend std::ostream& operator<<(std::ostream& os, const Instance& obj);

    private:
        const Tuple& tuple_;
        std::vector<BlockType> content_bit_blocks_;
        double group_size_ = 1;
        double dim_;
        int head_attr_ = 0;
        int attr_count_;

        void RemoveAttribute_(int index);

        std::string StringValueOf_(const Entry& e) const {
            return e.StringFromIntValue(IntValueOf_(e));
        }

        Entry::IntType IntValueOf_(const Entry& e) const {
            return (content_bit_blocks_[e.block_index()] & ~e.negative_mask()) >> e.block_offset();
        }

        void SetValueOf_(const Entry& e, Entry::IntType value) {
            content_bit_blocks_[e.block_index()] &= e.negative_mask();
            content_bit_blocks_[e.block_index()] |= value << e.block_offset();
        }

        void SetValueOf_(const Entry& e, const std::string& value) {
            SetValueOf_(e, e.IntValueFromString(value));
        }
    };

private:
    static inline int min_attribute_count_ = 1;
    static inline int max_attribute_count_ = std::numeric_limits<int>::max() - 1;

    std::vector<std::unique_ptr<Entry>> entries_;
    int index_of_target_;
    int current_block_ = 0;
    unsigned current_offset_ = 0;
    double dim_ = 1;
    bool closed_ = false;
};
} // cprior::encoder

template<>
struct std::hash<cprior::encoder::Tuple::Instance> {
    std::size_t operator()(const cprior::encoder::Tuple::Instance& tuple) const noexcept {
        return tuple.hash();
    }
};

#endif //CPRIOR_ENCODER_TUPLE_H
