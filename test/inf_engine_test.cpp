//
// Created by aybehrouz on 6/6/25.
//

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "encoder/DataSet.h"
#include "multinomial/InferenceEngine.h"
#include "tools/MockOutcome.h"

using namespace cprior::multinomial;
using namespace cprior::encoder;
using namespace testing;

TEST(InferenceEngineTest, CalculatesCentralProbabilities) {
    InferenceEngine<MockOutcome> engine;
    engine.AddEvidence(MockOutcome(3), 1);
    engine.AddEvidence(MockOutcome(4), 3);
    engine.AddEvidence(MockOutcome(6), 7);
    engine.AddEvidence(MockOutcome(8), 2);

    engine.ProcessEvidence();

    auto got = engine.ComputePosterior({{1}, {3}, {8}, {7},});
    std::vector expected = {0.093906757691225987, 0.23013424434611504, 0.57428073796151557, 0.10167826000114334};

    for (int i = 0; i < got.size(); ++i) {
        EXPECT_DOUBLE_EQ(got[i].to_double(), expected[i]) << "for i = " << i;
    }

    EXPECT_DOUBLE_EQ(engine.ComputePosterior({5})[0].to_double(), 9.1541000736655327e-12);
}

TEST(InfEngineTest, CalculatesBestOutcome) {
    Tuple t(0);
    t
            .AddNominalEntry("target", {"win", "lose"})
            .AddNominalEntry("att1", {"0", "1"})
            .AddNominalEntry("att2", {"0", "1"})
            .AddNominalEntry("att3", {"0", "1"});
    t.close();

    InferenceEngine<Tuple::Instance> engine;
    engine.AddEvidence({t, {"lose", "0", "0", "1"}});
    engine.AddEvidence({t, {"win", "0", "1", "0"}});
    engine.AddEvidence({t, {"win", "1", "0", "1"}});
    engine.AddEvidence({t, {"win", "1", "1", "0"}});
    engine.AddEvidence({t, {"lose", "0", "0", "0"}});


    Tuple::Instance q(t, {"lose", "0", "0", "0"});
    auto [query, _] = q.ComputeTargetInstances();

    engine.ProcessEvidence();

    auto answer = engine.MostProbableOutcome(query);
    std::cout << query.at(answer) << std::endl;

    std::cout << engine.MostProbableModel() << std::endl;

    //EXPECT_EQ(query.at(answer), Tuple::Instance(t, {"win", "1", "1", "1"}));
}
