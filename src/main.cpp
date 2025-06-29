#include "multinomial/InferenceEngine.h"
#include "../test/mock/SmallOutcome.h"
#include "encoder/DataGenerator.h"
#include "encoder/DataSet.h"
#include "multinomial/Evaluator.h"

using namespace cprior::multinomial;
using namespace cprior::encoder;

int main() {
    const std::set target_attributes{3,5};
    const std::string info = "gen.info", data = "gen.data";
    constexpr int attr_count = 14, data_set_size = 20;
    constexpr int trial_count = 200;

    DataGenerator dg({{"00", "A"}, {"01", "B"}, {"10", "C"}, {"11", "D"}}
                     , attr_count, target_attributes);

    dg.WriteInfoFile(info);
    dg.WriteDataFile(data, data_set_size);

    auto accuracy = Evaluator::EvaluateIncremental(info, data, target_attributes, trial_count);
    std::cout << "c(" << accuracy[0];

    for (int i = 1; i < accuracy.size(); ++i) {
        std::cout << "," << accuracy[i];
    }
    std::cout << ")" << std::endl;
    return 0;
}
