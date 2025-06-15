//
// Created by aybehrouz on 10/25/24.
//

#ifndef CPRIOR_CONSTANT_SUM_RANGE_H
#define CPRIOR_CONSTANT_SUM_RANGE_H
#include <vector>

namespace cprior::util {
class ConstantSumRange {
public:
    class Iterator {
    public:
        using input_type = std::vector<int>;
        using value_type = input_type;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type *;
        using reference = value_type &;
        using iterator_category = std::input_iterator_tag;

        explicit Iterator(input_type current) : current_(std::move(current)) {
        }

        value_type operator*() const {
            return current_;
        }


        Iterator& operator++();

        void operator++(int) {
            ++*this;
        }

        friend bool operator==(const Iterator& lhs, const Iterator& rhs) {
            return lhs.current_[lhs.current_.size() - 1] == rhs.current_[rhs.current_.size() - 1] &&
                   lhs.current_ == rhs.current_;
        }

        friend bool operator!=(const Iterator& lhs, const Iterator& rhs) {
            return !(lhs == rhs);
        }

    private:
        input_type current_;
        std::size_t current_index_ = 0;
    };


    ConstantSumRange(int sum_start, int sum_end, int number_of_elements);

    ConstantSumRange(int sum, int number_of_elements) : ConstantSumRange(sum, sum, number_of_elements) {
    }


    Iterator begin() {
        return Iterator(begin_);
    }

    Iterator end() {
        return Iterator(end_);
    }

private:
    Iterator::input_type begin_;
    Iterator::input_type end_;
};
} // cprior::util

#endif //CPRIOR_CONSTANT_SUM_RANGE_H
