//
// Created by aybehrouz on 6/14/25.
//

#include <gtest/gtest.h>

#include "encoder/DataSet.h"

using namespace cprior::encoder;

TEST(DatasetTest, ReadsFromFile) {
    DataSet d;
    d.ReadFile("data.txt");
}