//
// Created by aybehrouz on 5/21/25.
//

#ifndef GAMMA_H
#define GAMMA_H
#include "HpFloat.h"
#define GAMMA_PRECOMPUTE_FILE "gamma_precompute_values.inc"

namespace cprior::math {

class Gamma {
public:
    static constexpr int kCacheSize = 1024;

    static double logLimit(double z);

    static HpFloat logHalfInt(unsigned long n);

    static HpFloat logInt(unsigned long n);


    static void writePrecomputeHeaderFile();

private:
#include GAMMA_PRECOMPUTE_FILE
     // static inline HpFloat precompute_n_0_[]{};
     // static inline HpFloat precompute_n_1_2_[]{};
};

} // cprior::math


#endif // GAMMA_H
