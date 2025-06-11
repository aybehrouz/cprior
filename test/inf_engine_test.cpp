//
// Created by aybehrouz on 6/6/25.
//

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include "multinomial/InferenceEngine.h"
#include "mock/MockOutcome.h"

using namespace cprior::multinomial;
using namespace testing;

TEST(InferenceEngineTest, CalculatesCentralProbabilities) {
    InferenceEngine<MockOutcome> engine;
    engine
            .addEvidence(MockOutcome(3), 1)
            .addEvidence(MockOutcome(4), 3)
            .addEvidence(MockOutcome(6), 7)
            .addEvidence(MockOutcome(8), 2)

            .processEvidence();

    auto got = engine.computePosterior({{1}, {3}, {8}, {7},});
    std::vector expected = {0.093906757691225987, 0.23013424434611504, 0.57428073796151557, 0.10167826000114334};

    for (int i = 0; i < got.size(); ++i) {
        EXPECT_DOUBLE_EQ(got[i].to_double(), expected[i]) << "for i = " << i;
    }

    EXPECT_DOUBLE_EQ(engine.computePosterior({5})[0].to_double(), 9.1541000736655327e-12);


}
