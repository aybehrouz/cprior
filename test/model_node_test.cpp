//
// Created by aybehrouz on 6/11/25.
//

#include <gtest/gtest.h>

#include "mock/MockOutcome.h"
#include "mock/SmallOutcome.h"
#include "multinomial/ModelNode.h"
#include "multinomial/Variable.h"

using namespace cprior::multinomial;

TEST(ModelNodeTest, CalculatesWeightedSums) {
    Variable<MockOutcome> observations;
    observations.add({5}, 3);
    observations.add({2}, 1);
    observations.add({7}, 5);

    ModelNode root(observations);
    auto node_count = root.createSubTree();
    EXPECT_DOUBLE_EQ(root.getTreeWeightedSum().to_double(), 4.0651172060858136e-07);

    EXPECT_DOUBLE_EQ(root.getTreeWeightedSum().to_double() / node_count, 1.0162793015214534e-07);

}

TEST(ModelNodeTest, UpdatesObservations) {
    Variable<MockOutcome> observations;
    observations.add({1}, 3);
    observations.add({4}, 6);
    observations.add({6}, 4);

    ModelNode root(observations);
    auto node_count = root.createSubTree();


    Variable<MockOutcome> delta;
    delta.add({1}, -1);
    delta.add({4}, 4);
    delta.add({3}, 2);
    delta.add({6}, -4);
    delta.add({8}, 1);
    delta.add({1}, -1);
    delta.add({4}, -2);


    // {1,0,2,8,0,0,0,1}
    root.updateObservations(delta);

    EXPECT_DOUBLE_EQ(root.getTreeWeightedSum().to_double() / node_count , 1.9692160029600339e-09);

    EXPECT_DOUBLE_EQ(root.getNodeProbability().to_double(), 6.8119594029017557e-09);
}


TEST(ModelNodeTest, CreatesTree) {
    auto root = ModelNode<SmallOutcome>::CreateModelTree(150, 1);
}

TEST(ModelNodeTest, HandlesLargeN) {
    Variable<MockOutcome> observations;

    observations.add({3}, 70);
    observations.add({4}, 50);
    observations.add({6}, 60);

    ModelNode root(observations);
    const auto n = root.createSubTree();

    EXPECT_DOUBLE_EQ(root.getNodeProbability().to_double() / n, 2.3606648986036229e-63);
}

using namespace cprior::util;
TEST(UtilTest, Gamma) {
    EXPECT_DOUBLE_EQ(Gamma::MultiChoose({2,3,0,1}).to_double(), 60);

    EXPECT_DOUBLE_EQ(Gamma::MultiChoose({5,0,10}).to_double(), 3003);

    EXPECT_DOUBLE_EQ(Gamma::MultiChoose({0,0,0}).to_double(), 1);
}
