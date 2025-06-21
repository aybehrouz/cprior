//
// Created by aybehrouz on 6/14/25.
//

#include <gtest/gtest.h>

#include "encoder/DataSet.h"
#include "multinomial/Evaluator.h"

using namespace cprior::encoder;
using namespace cprior::multinomial;

TEST(DatasetTest, ReadsFromFile) {

    auto t = DataSet::ReadInfoFile("info.txt");

    DataSet d(t, "data.txt");

    EXPECT_EQ(*d.begin(), Tuple::Instance(t, {"a","female","win"}));
    EXPECT_EQ(*++d.begin(), Tuple::Instance(t, {"c","male","lose"}));
    EXPECT_EQ(*++++d.begin(), Tuple::Instance(t, {"d","female","win"}));
}

TEST(EvaluatorTest, CalculatesAccuracy_binary3) {
    Evaluator evaluator("binary3.info");

    for (int i = 0; i < 200; ++i) {
        evaluator.Evaluate("binary3.data");
    }
    EXPECT_NEAR(evaluator.accuracy(), 0.9, 0.01);
}


TEST(EvaluatorTest, CalculatesAccuracy_bin5) {
    Evaluator bin5("bin5.info");

    for (int i = 0; i < 300; ++i) {
        bin5.Evaluate("bin5.data");
    }
    EXPECT_NEAR(bin5.accuracy(), 0.22, 0.01);
}

