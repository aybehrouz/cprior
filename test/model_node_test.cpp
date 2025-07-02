//
// Created by aybehrouz on 6/11/25.
//

#include <gtest/gtest.h>

#include "encoder/Tuple.h"
#include "tools/MockOutcome.h"
#include "tools/SmallOutcome.h"
#include "multinomial/ModelNode.h"
#include "multinomial/Variable.h"

using namespace cprior::multinomial;
using namespace cprior::encoder;

TEST(ModelNodeTest, CalculatesWeightedSums) {
    Variable<MockOutcome> observations;
    observations.Add({5}, 3);
    observations.Add({2}, 1);
    observations.Add({7}, 5);

    ModelNode root(observations);
    auto node_count = root.CreateSubTree();
    EXPECT_DOUBLE_EQ(root.TreeWeightedSum().to_double(), 4.0651172060858136e-07);

    EXPECT_DOUBLE_EQ(root.TreeWeightedSum().to_double() / node_count, 1.0162793015214534e-07);

    Variable<Tuple::Instance> v;
    Tuple t(2);
    t
            .AddNominalEntry("att1", {"false", "true"})
            .AddNominalEntry("att2", {"a", "b", "c"})
            .AddNominalEntry("target", {"win", "lose"});
    t.close();
    v.Add({t, {"false", "a", "win"}}, 3);
    v.Add({t, {"true", "c", "lose"}}, 2);

    ModelNode r(v);
    r.CreateSubTree();
    EXPECT_DOUBLE_EQ(r.TreeWeightedSumAfter({t, {"false", "c", "win"}}).to_double(), 3.998320303975322e-06);
}


TEST(ModelNodeTest, UpdatesObservations) {
    Variable<MockOutcome> observations;
    observations.Add({1}, 3);
    observations.Add({4}, 6);
    observations.Add({6}, 4);

    ModelNode root(observations);
    auto node_count = root.CreateSubTree();


    Variable<MockOutcome> delta;
    delta.Add({1}, -1);
    delta.Add({4}, 4);
    delta.Add({3}, 2);
    delta.Add({6}, -4);
    delta.Add({8}, 1);
    delta.Add({1}, -1);
    delta.Add({4}, -2);


    // {1,0,2,8,0,0,0,1}
    root.UpdateObservations(delta);

    EXPECT_DOUBLE_EQ(root.TreeWeightedSum().to_double() / node_count , 1.9692160029600339e-09);

    EXPECT_DOUBLE_EQ(root.NodeProbability().to_double(), 6.8119594029017557e-09);
}


TEST(ModelNodeTest, CreatesTree) {
    auto root = ModelNode<SmallOutcome>::CreateModelTree(150, 1);
}

TEST(ModelNodeTest, HandlesLargeN) {
    Variable<MockOutcome> observations;

    observations.Add({3}, 70);
    observations.Add({4}, 50);
    observations.Add({6}, 60);

    ModelNode root(observations);
    const auto n = root.CreateSubTree();

    EXPECT_DOUBLE_EQ(root.NodeProbability().to_double() / n, 2.3606648986036229e-63);
}

using namespace cprior::util;
TEST(UtilTest, Gamma) {
    EXPECT_EQ(std::lround(Gamma::MultiChoose({2,3,0,1}).to_double()), 60);

    EXPECT_DOUBLE_EQ(std::lround(Gamma::MultiChoose({5,0,10}).to_double()), 3003);

    EXPECT_DOUBLE_EQ(std::lround(Gamma::MultiChoose({0,0,0}).to_double()), 1);
}
