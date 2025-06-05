#include <cmath>
#include <iostream>
#include <regex>

#include "src/encoder/AttributeTuple.h"
#include "src/multinomial/InferenceEngine.h"
#include "src/multinomial/MockOutcome.h"
#include "src/multinomial/Variable.h"
#include "src/util/ConstantSumRange.h"

using namespace cprior;

int main() {
    /*
        encoder::AttributeType t1("first", {"1", "this", "that", "ops"});
        encoder::AttributeType t2("second", {"10", "20", "30"});
        encoder::AttributeType t3("three", {"a", "b"});

        encoder::AttributeSet s;
        s.addAttribute(&t1).
        s.addAttribute(&t2);

        encoder::AttributeTuple at("ops   ,  30", s);

        std::cout << at << std::endl;
        std::cout << at << "$$" << t2 << "\n";
        std::cout << at.stringFromIntValue(3) << " " << t.stringFromIntValue(0) << "\n";
    */
    util::ConstantSumRange r(5, 5, 4);
    for (const auto& v: r) {
        std::cout << v[0] << " " << v[1] << " " << v[2] << " " << v[3] << ":" << v.size() << std::endl;
    }
    multinomial::InferenceEngine<MockOutcome> eng;
    eng.addEvidence(MockOutcome(2), 2)
            .addEvidence(MockOutcome(3), 1)
            .addEvidence(MockOutcome(5), 3)
            .addEvidence(MockOutcome(7), 1)
            .addEvidence(MockOutcome(8), 2);

    eng.processEvidence();
    auto p = eng.computeProbability({
        {1}, {2}, {3}, {4},
        {5}, {6}, {7}, {8},
    });

    multinomial::Variable<MockOutcome> dif;
    dif.add(MockOutcome(1), 2);
    dif.add(MockOutcome(5), -3);
    dif.add(MockOutcome(8), 3);


    std::cout << p[0] << " " << p[1] << " " << p[6] << " " << std::endl;
    std::cout << eng << std::endl;
    eng.update(dif);
    std::cout << eng << std::endl;


    // Tokenization (non-matched fragments)
    // Note that regex is matched only two times; when the third value is obtained
    // the iterator is a suffix iterator.
    const std::string text = "Quick  ,,  brown, \t  fox.";
    const std::regex ws_re("(\\s|,)+"); // whitespace and ,
    for (auto token = std::sregex_token_iterator(text.begin(), text.end(), ws_re, -1);
         token != std::sregex_token_iterator(); ++token) {
        std::cout << *token << '\n';
    }
    return 0;
}
