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

    EXPECT_EQ(t.entry_count(), 4);

    EXPECT_EQ(t.entry(0).StringFromIntValue(1), "a");
    EXPECT_EQ(t.entry(0).cardinality(), 4);
    EXPECT_EQ(t.entry(0).bit_len(), 3);
    std::cout << t.entry(0) << std::endl;

    EXPECT_EQ(t.entry(1).StringFromIntValue(1), "male");
    EXPECT_EQ(t.entry(1).negative_mask(), 0xffffffffffffffe7);
    EXPECT_EQ(t.entry(1).cardinality(), 2);
    std::cout << t.entry(1) << std::endl;

    EXPECT_EQ(t.entry(2).StringFromIntValue(10), "1.500000");
    EXPECT_EQ(t.entry(2).StringFromIntValue(1), "-1.000000");
    EXPECT_EQ(t.entry(2).StringFromIntValue(7), "0.666667");
    EXPECT_EQ(t.entry(2).cardinality(), 10);
    std::cout << t.entry(2) << std::endl;

    EXPECT_EQ(t.entry(3).StringFromIntValue(2), "lose");
    EXPECT_EQ(t.entry(3).cardinality(), 2);
    std::cout << t.entry(3) << std::endl;

    DataSet d(t, "data.txt");

    EXPECT_EQ(*d.begin(), Tuple::Instance(t, {"a","female","-1.000000","win"}));
    EXPECT_EQ(*++d.begin(), Tuple::Instance(t, {"c","male","1.222222","lose"}));
    EXPECT_EQ(*++++d.begin(), Tuple::Instance(t, {"d","female","-0.166667","win"}));
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

