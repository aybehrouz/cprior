//
// Created by aybehrouz on 5/21/25.
//

#ifndef HP_FLOAT_H
#define HP_FLOAT_H
#include <iomanip>
#include <string>
#include <cmath>

namespace cprior::util {
class HpFloat {
public:
    HpFloat() = default;

    HpFloat(double val) : val_(val) {
    }

    HpFloat(const char *decimal) : val_(std::stod(decimal)) {
    }

    [[nodiscard]]
    std::string to_string() const {
        std::ostringstream oss;
        oss << std::scientific << std::setprecision(21) << val_;
        return oss.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const HpFloat& obj) {
        return os << obj.to_string();
    }

    HpFloat operator+(const HpFloat& other) const {
        return {val_ + other.val_};
    }

    HpFloat operator-(const HpFloat& other) const {
        return {val_ - other.val_};
    }

    HpFloat operator/(const HpFloat& other) const {
        return {val_ / other.val_};
    }

    HpFloat operator*(const HpFloat& other) const {
        return {val_ * other.val_};
    }

    [[nodiscard]]
    HpFloat exp() const {
        return {std::exp(val_)};
    }

    [[nodiscard]]
    HpFloat log() const {
        return {std::log(val_)};
    }


    HpFloat& operator+=(const HpFloat& other) {
        val_ += other.val_;
        return *this;
    }

    HpFloat& operator-=(const HpFloat& other) {
        val_ -= other.val_;
        return *this;
    }

    HpFloat& operator*=(const HpFloat& other) {
        val_ *= other.val_;
        return *this;
    }

    HpFloat& operator/=(const HpFloat& other) {
        val_ /= other.val_;
        return *this;
    }

private:
    double val_{};
};
}

#endif // HP_FLOAT_H
