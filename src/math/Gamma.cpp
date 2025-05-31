//
// Created by aybehrouz on 5/21/25.
//

#include "Gamma.h"

#include <fstream>
#include <cmath>

namespace cprior::math {


double Gamma::logLimit(double z) {
    return (z - 0.5) * std::log(z) - z + std::log(2 * M_PI) / 2;
}

HpFloat Gamma::logHalfInt(unsigned long n) {
    if (n <= kCacheSize - 1) {
        return precompute_n_1_2_[n];
    }
    return logLimit(static_cast<double>(n) + 0.5);
}

HpFloat Gamma::logInt(unsigned long n) {
    if (n <= kCacheSize - 1) {
        return precompute_n_0_[n];
    }
    return logLimit(static_cast<double>(n));
}

void Gamma::writePrecomputeHeaderFile() {
    std::ofstream header_file(GAMMA_PRECOMPUTE_FILE);
    if (!header_file.is_open()) throw std::runtime_error("failed to open header file");

    header_file << "static inline HpFloat precompute_n_0_[kCacheSize] = {\n";
    for (int i = 0; i < kCacheSize; ++i) {
        header_file << "\t\"" << HpFloat(std::lgamma(i)) << "\", \n";
    }
    header_file << "}; \n \n";

    header_file << "static inline HpFloat precompute_n_1_2_[kCacheSize] = {\n";
    for (int i = 0; i < kCacheSize; ++i) {
        header_file << "\t\"" << HpFloat(std::lgamma(i + 0.5)) << "\", \n";
    }
    header_file << "}; \n";
}





} // cprior::math
