#include "multinomial/InferenceEngine.h"
#include "../test/mock/SmallOutcome.h"

using namespace cprior;

int main() {
    auto root = multinomial::ModelNode<SmallOutcome>::CreateModelTree(440, 2);
    return 0;
}
