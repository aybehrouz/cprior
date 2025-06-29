//
// Created by aybehrouz on 5/21/25.
//

#ifndef GAMMA_H
#define GAMMA_H
#include <vector>
#include "HpFloat.h"


#define GAMMA_PRECOMPUTE_FILE "gamma_precompute_values.inc"

namespace cprior::util {

class Gamma {
public:
    static constexpr int kCacheSize = 1 << 12;

    static double lgammaLimit(double z);

    static HpFloat lgammaIntPlusHalf(unsigned long n);

    static HpFloat lgammaInt(unsigned long n);


    static void writePrecomputeHeaderFile();

    static HpFloat MultiChoose(const std::vector<int>& x);

private:
#include GAMMA_PRECOMPUTE_FILE
     // static inline HpFloat precompute_n_0_[]{};
     // static inline HpFloat precompute_n_1_2_[]{};
};

} // cprior::math


#endif // GAMMA_H
