//
// Created by aybehrouz on 6/13/25.
//


#include <gtest/gtest.h>

#include "encoder/Tuple.h"

using namespace cprior::encoder;

TEST(TupleTest, AddsEntries) {
    Tuple tuple(1);

    tuple
            .AddNominalEntry({"Type", "a", "b", "c", "d"})
            .AddNominalEntry({"Three", "1", "2", "3"})
            .AddOrdinalEntry({"Age", "100", "1", "100"})
            .AddOrdinalEntry({"Big", "10000000", "0", "1"})
            .AddOrdinalEntry({"Float", "400000000", "-1", "1"})
            .AddNominalEntry({"Answer", "yes", "no"})
            .AddNominalEntry({"Small", "one"});

    tuple.close();
    Tuple::Instance sample(tuple, {"c", "2", "3", "0.3687", "-0.512", "no", "one"});


    std::cout << sample << std::endl;

    EXPECT_EQ(sample.attribute_str(0), "c");
    EXPECT_EQ(sample.attribute_str(1), "3.000000");
    EXPECT_EQ(sample.attribute_str(2), "0.368700");
    EXPECT_EQ(sample.attribute_str(3), "-0.512000");
    EXPECT_EQ(sample.attribute_str(4), "no");
    EXPECT_EQ(sample.attribute_str(5), "one");
}

TEST(TupleTest, ComputesPossibleTargetInstances) {
    Tuple tuple(2);
    tuple
            .AddNominalEntry({"first", "1", "2"})
            .AddNominalEntry({"second", "x", "y", "z"})
            .AddNominalEntry({"target", "A", "B", "C", "D", "E"})
            .AddNominalEntry({"third", "1", "2", "3", "4"});

    tuple.close();
    Tuple::Instance sample(tuple, {"1", "z", "C", "3"});

    auto domain = sample.ComputeTargetInstances();
    ASSERT_EQ(domain.size(), 5);
    EXPECT_EQ(domain[0], Tuple::Instance(tuple, {"1", "z", "C", "3"}));
    EXPECT_EQ(domain[1], Tuple::Instance(tuple, {"1", "z", "A", "3"}));
    EXPECT_EQ(domain[2], Tuple::Instance(tuple, {"1", "z", "B", "3"}));
    EXPECT_EQ(domain[3], Tuple::Instance(tuple, {"1", "z", "D", "3"}));
    EXPECT_EQ(domain[4], Tuple::Instance(tuple, {"1", "z", "E", "3"}));

    domain = Tuple::Instance(tuple, {"1", "z", "A", "3"}).ComputeTargetInstances();
    ASSERT_EQ(domain.size(), 5);
    EXPECT_EQ(domain[0], Tuple::Instance(tuple, {"1", "z", "A", "3"}));
    EXPECT_EQ(domain[1], Tuple::Instance(tuple, {"1", "z", "B", "3"}));
    EXPECT_EQ(domain[2], Tuple::Instance(tuple, {"1", "z", "C", "3"}));
    EXPECT_EQ(domain[3], Tuple::Instance(tuple, {"1", "z", "D", "3"}));
    EXPECT_EQ(domain[4], Tuple::Instance(tuple, {"1", "z", "E", "3"}));


}

int CountReductions(const Tuple::Instance& instance) {
    int count = instance.num_of_reductions();
    for (const auto& r: instance.ComputeReductions()) {
        count += CountReductions(r);
    }
    std::cout << instance << std::endl;
    return count;
}

TEST(TupleInstanceTest, ComputesReductions) {
    Tuple tuple(2);
    tuple
            .AddNominalEntry({"first", "a", "b", "c"})
            .AddNominalEntry({"second", "true", "false"})
            .AddNominalEntry({"target", "win", "lose"})
            .AddNominalEntry({"third", "1", "2", "3", "4"});

    tuple.close();
    Tuple::Instance sample(tuple, {"b", "true", "win", "4"});

    EXPECT_EQ(CountReductions(sample), 6);

    auto r = sample.ComputeReductions();

    EXPECT_EQ(sample.num_of_reductions(), 3);
    EXPECT_EQ(r.size(), 3);

    EXPECT_EQ(r[0].dim(), 2*2*4);
    EXPECT_EQ(r[0].group_size(), 3);

    auto r_0 = r[0].ComputeReductions();
    EXPECT_EQ(r[0].num_of_reductions(), 2);
    EXPECT_EQ(r_0.size(), 2);

    EXPECT_EQ(r_0[0].dim(), 2*4);
    EXPECT_EQ(r_0[0].group_size(), 6);


    EXPECT_EQ(r[1].dim(), 3*2*4);
    EXPECT_EQ(r[1].group_size(), 2);
}
