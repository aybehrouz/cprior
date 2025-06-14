#include <cmath>
#include <iostream>
#include <regex>

#include "encoder/AttributeTuple.h"
#include "multinomial/InferenceEngine.h"
#include "../test/mock/MockOutcome.h"
#include "../test/mock/SmallOutcome.h"
#include "multinomial/Variable.h"
#include "util/ConstantSumRange.h"

using namespace cprior;

int main() {
    auto root = multinomial::ModelNode<SmallOutcome>::CreateModelTree(440, 2);
    return 0;
}
