//
// Created by aybehrouz on 6/14/25.
//

#include <gtest/gtest.h>

#include "encoder/DataGenerator.h"
#include "encoder/DataSet.h"
#include "multinomial/Evaluator.h"

using namespace cprior::encoder;
using namespace cprior::multinomial;

TEST(DatasetTest, ReadsFromFile) {

    DataSet ds;

    auto t = ds.ReadInfoFile("data/info.txt");

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

    ds.ReadDataFile(t, "data/data.txt");

    EXPECT_EQ(*ds.begin(), Tuple::Instance(t, {"a","female","-1.000000","win"}));
    EXPECT_EQ(*++ds.begin(), Tuple::Instance(t, {"c","male","1.222222","lose"}));
    EXPECT_EQ(*++++ds.begin(), Tuple::Instance(t, {"d","female","-0.166667","win"}));

}

TEST(DataSetTest, CanIgnoreEntries) {
    DataSet ds({0,3});
    auto t = ds.ReadInfoFile("data/info.txt");
    std::cout << t.to_string() << std::endl;
    EXPECT_EQ(t.to_string(), "0\nNominal gender,female,male,\nOrdinal temp,10,-1,1.5\n");

    ds.ReadDataFile(t, "data/data.txt");
    EXPECT_EQ(*ds.begin(), Tuple::Instance(t, {"female","-1.000000"}));
    EXPECT_EQ(*++ds.begin(), Tuple::Instance(t, {"male","1.222222"}));

}

TEST(EvaluatorTest, CalculatesAccuracy_binary3) {
    DataSet ds;
    auto tuple = ds.ReadInfoFile("data/binary3.info");
    ds.ReadDataFile(tuple, "data/binary3.data");

    EXPECT_NEAR(Evaluator(ds).Evaluate(50), 0.98, 0.02);
}

TEST(EvaluatorTest, CalculatesAccuracy_loan) {
    DataSet ds;
    auto tuple = ds.ReadInfoFile("data/loan.info");
    ds.ReadDataFile(tuple, "data/loan.csv");

    EXPECT_NEAR(Evaluator(ds).Evaluate(50), 0.98, 0.02);
}

TEST(DataSetTest, DataGen) {
    DataGenerator dg({{"00", "zero"}, {"01", "zero"}, {"10", "zero"}, {"11", "one"}}
                     , 8, {3, 4});
    std::cout << dg.Generate() << std::endl;
    std::cout << dg.Generate() << std::endl;


    dg.WriteInfoFile("data/gen.info");
    dg.WriteDataFile("data/gen.data", 20);

    DataSet ds;
    auto tuple = ds.ReadInfoFile("data/gen.info");
    ds.ReadDataFile(tuple, "data/gen.data");

    EXPECT_NEAR(Evaluator(ds).Evaluate(50), 0.79, 0.02);
}


TEST(EvaluatorTest, EvaluatesRemovingAttributes) {
    const std::set target_attributes{0,6};
    const std::string info = "data/gen.info", data = "data/gen.data";
    constexpr int attr_count = 7, data_set_size = 10;
    constexpr int trial_count = 1;


    DataGenerator dg({{"00", "one"}, {"01", "zero"}, {"10", "zero"}, {"11", "one"}}
                     , attr_count, target_attributes);

    dg.WriteInfoFile(info);
    dg.WriteDataFile(data, data_set_size);

    auto acc = Evaluator<>::EvaluateIncremental(info, data, target_attributes, trial_count);
    EXPECT_EQ(acc.size(), attr_count - target_attributes.size() + 1);
}

TEST(EvaluatorTest, AnswersQueries) {
    const std::set target_attributes{0,4,7};
    const std::string info = "data/data_q.info", train_file = "data/train_q.data", query_file = "data/query_q.data";
    constexpr int attr_count = 14, train_size = 20, query_size = 7;


    DataGenerator generator({
                         {"000", "A"}, {"001", "B+"}, {"010", "C"}, {"011", "D"},
                         {"100", "AA"}, {"101", "BB"}, {"110", "B+"}, {"111", "DD"}
                     }
                     , attr_count, target_attributes);

    generator.WriteInfoFile(info);
    generator.WriteDataFile(train_file, train_size);
    generator.WriteDataFile(query_file, query_size);

    DataSet train;
    auto tuple_info = train.ReadInfoFile(info);
    train.ReadDataFile(tuple_info, train_file);

    DataSet query;
    query.ReadDataFile(tuple_info, query_file);

    Evaluator eval(train);
    eval.Answer(query);
}
