//
// Created by aybehrouz on 6/14/25.
//

#include <gtest/gtest.h>

#include "encoder/DataSet.h"
#include "multinomial/Evaluator.h"

using namespace cprior::encoder;

TEST(DatasetTest, ReadsFromFile) {

    auto t = DataSet::ReadInfoFile("info.txt");

    DataSet d(t, "data.txt");

    for (const auto & datum: d) {
        std::cout << datum << std::endl;
    }

    cprior::multinomial::Evaluator::Eval();
}
