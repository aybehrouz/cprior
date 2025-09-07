//
// Created by aybehrouz on 6/1/25.
//

#ifndef CPRIOR_ENCODER_TUPLE_H
#define CPRIOR_ENCODER_TUPLE_H

#include <utility>
#include <limits>

#include "Entry.h"
#include "util/Gamma.h"

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

    Tuple& AddNominalEntry(std::string name, std::vector<std::string> categories) {
        return this->AddEntry(std::make_unique<NominalEntry>(std::move(name), std::move(categories)));
    }

    Tuple& AddOrdinalEntry(std::string name, unsigned cardinality, double min, double max) {
        return this->AddEntry(std::make_unique<OrdinalEntry>(std::move(name), cardinality, min, max));
    }

    [[nodiscard]]
    const Entry& entry(int index) const {
        return *entries_[index];
    }

    [[nodiscard]]
    const Entry& target() const {
        return *entries_[index_of_target_];
    }

    [[nodiscard]]
    int index_of_target() const {
        return index_of_target_;
    }

    [[nodiscard]]
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

    static void ChangeMaxAttributes(int max = std::numeric_limits<int>::max() - 1) {
        max_attributes_ = max;
    }

    static void ChangeMinAttributes(int min = 1) {
        min_attributes_ = min;
    }

    class Instance {
    public:
        Instance(const Tuple& tuple, const std::vector<std::string>& tokens);

        [[nodiscard]]
        std::vector<Instance> ComputeReductions() const;

        [[nodiscard]]
        std::pair<std::vector<Instance>, Entry::IntType> ComputeTargetInstances() const;

        [[nodiscard]]
        std::string attribute_str(int index) const {
            return StringValueOf_(tuple_.attribute(index));
        }

        [[nodiscard]]
        std::string attribute_str() const;

        [[nodiscard]]
        std::string target_str() const {
            return StringValueOf_(tuple_.target());
        }

        [[nodiscard]]
        auto target_int_value() const {
            return IntValueOf_(tuple_.target());
        }

        [[nodiscard]]
        auto target_cardinality() const {
            return tuple_.target().cardinality();
        }

        void set_target(Entry::IntType value) {
            SetValueOf_(tuple_.target(), value);
        }

        void RemoveTarget();

        [[nodiscard]]
        int num_of_reductions() const {
            if (attr_count_ <= min_attributes_) return 0;

            if (attr_count_ > max_attributes_ + 1) {
                return int(util::Gamma::MultiChoose({max_attributes_, attr_count_ - max_attributes_}));
            }

            return tuple_.attribute_count() - head_attr_;
        }

        [[nodiscard]]
        double group_size() const {
            return group_size_;
        }

        [[nodiscard]]
       double preimage_size() const {
            return group_size_;
        }

        [[nodiscard]]
       int attribute_count() const {
            return attr_count_;
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
            if (lhs.content_bit_blocks_ == rhs.content_bit_blocks_) return true;
            return false;
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

        [[nodiscard]]
        std::string StringValueOf_(const Entry& e) const {
            return e.StringFromIntValue(IntValueOf_(e));
        }

        [[nodiscard]]
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
    static inline int min_attributes_ = 1;
    static inline int max_attributes_ = std::numeric_limits<int>::max() - 1;

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
