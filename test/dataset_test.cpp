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

TEST(EvaluatorTest, CalculatesAccuracy) {
    Evaluator evaluator("binary3.info");

    for (int i = 0; i < 1000; ++i) {
        evaluator.Evaluate("binary3.data");
    }
    EXPECT_NEAR(evaluator.accuracy(), 0.97, 0.01);
}
