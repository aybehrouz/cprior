//
// Created by aybehrouz on 6/4/25.
//

#ifndef MOCK_OUTCOME_H
#define MOCK_OUTCOME_H

#include <cassert>
#include <ostream>
#include <vector>


class MockOutcome {
public:
    MockOutcome(int lvl, int content)
        : lvl_(lvl),
          content_(content) {
    }

    MockOutcome(int content)
       : lvl_(0),
         content_(content) {
    }

    static std::vector<MockOutcome> possibleOutcomes() {
        return {{1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}};
    }

    int num_of_reductions() const {
        if (lvl_ == 0) return 1;
        if (lvl_ == 1) return 2;
        if (lvl_ == 2) return 0;
        return -1;
    }

    double dim() const {
        if (lvl_ == 0) return 8;
        if (lvl_ == 1) return 4;
        if (lvl_ == 2) return 2;
        return -1;
    }

    std::vector<MockOutcome> ComputeReductions() const {
        if (lvl_ == 0) return {MockOutcome(1, content_ % 4)};
        if (lvl_ == 1) return {MockOutcome(2, content_ % 2), MockOutcome(2, content_ / 2)};\
        if (lvl_ == 2) return {};
        return {};
    }

    int group_size() const {
        if (lvl_ == 0) return 1;
        if (lvl_ == 1) return 2;
        if (lvl_ == 2) return 4;
        return -1;
    }

    friend bool operator==(const MockOutcome& lhs, const MockOutcome& rhs) {
        assert(lhs.lvl_ == rhs.lvl_);
        return lhs.content_ == rhs.content_;
    }

    friend std::ostream& operator<<(std::ostream& os, const MockOutcome& obj) {
        return os << " #: " << obj.content_;
    }

    [[nodiscard]] int content() const {
        return content_;
    }

private:
    int lvl_;
    int content_;
};

template<>
struct std::hash<MockOutcome> {
    std::size_t operator()(const MockOutcome& m) const noexcept {
        return m.content();
    }
};

#endif //MOCK_OUTCOME_H
