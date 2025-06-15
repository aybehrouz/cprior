//
// Created by aybehrouz on 6/12/25.
//

#ifndef SMALL_OUTCOME_H
#define SMALL_OUTCOME_H

#include <cassert>
#include <ostream>
#include <vector>


class SmallOutcome {
public:
    SmallOutcome(int lvl, int content)
       : lvl_(lvl),
         content_(content) {
    }

    SmallOutcome(int content)
       : lvl_(0),
         content_(content) {
    }

    static std::vector<SmallOutcome> possibleOutcomes() {
        return {{1}, {2}, {3}, {4}};
    }

    int num_of_reductions() const {
        if (lvl_ == 0) return 2;
        if (lvl_ == 1) return 0;
        return -1;
    }

    double dim() const {
        if (lvl_ == 0) return 4;
        if (lvl_ == 1) return 2;
        return -1;
    }

    std::vector<SmallOutcome> ComputeReductions() const {
        if (lvl_ == 0) return {SmallOutcome(1, content_ % 2), SmallOutcome(1, (content_ + 1) / 2)};
        if (lvl_ == 1) return {};
        return {};
    }

    int group_size() const {
        if (lvl_ == 0) return 1;
        if (lvl_ == 1) return 2;
        return -1;
    }

    friend bool operator==(const SmallOutcome& lhs, const SmallOutcome& rhs) {
        assert(lhs.lvl_ == rhs.lvl_);
        return lhs.content_ == rhs.content_;
    }

    friend std::ostream& operator<<(std::ostream& os, const SmallOutcome& obj) {
        return os << " $: " << obj.content_;
    }

    [[nodiscard]] int content() const {
        return content_;
    }

private:
    int lvl_;
    int content_;

};


template<>
struct std::hash<SmallOutcome> {
    std::size_t operator()(const SmallOutcome& m) const noexcept {
        return m.content();
    }
};


#endif //SMALL_OUTCOME_H
