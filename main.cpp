#include <cmath>
#include <iostream>

#include "src/math/Gamma.h"
#include "src/multinomial/InferenceNode.h"

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {
    // TIP Press <shortcut actionId="RenameElement"/> when your caret is at the <b>lang</b> variable name to see how CLion can help you rename it.
    cprior::math::Gamma::writePrecomputeHeaderFile();



    cprior::multinomial::AttributeTuple a({1,1,0}, true);
    std::cout << a.num_of_reductions() << "\n";
    cprior::multinomial::Variable v;
    v.add({{1,1,0},false} , 3);
    v.add({{1,0,1},false} , 5);
    v.add({{1,0,0},true} , 2);

    std::cout << v << "\n";

    cprior::multinomial::InferenceNode inode(v);

    inode.createSubTree();
    std::cout << inode.getProbabilityAfter({{1,0,1}, false}) << "\n";
    std::cout << inode.getSubTreeMeasureAfter({{1, 0, 1}, false}) << "\n";

    std::cout << inode << std::endl;
    std::cout << std::lgamma(12345) << "\t" <<cprior::math::Gamma::logInt(12345) << "!\n";

    for (int i = 1; i <= 5; i++) {
        // TIP Press <shortcut actionId="Debug"/> to start debugging your code. We have set one <icon src="AllIcons.Debugger.Db_set_breakpoint"/> breakpoint for you, but you can always add more by pressing <shortcut actionId="ToggleLineBreakpoint"/>.
        std::cout << "i = " << i << std::endl;
    }

    return 0;
    // TIP See CLion help at <a href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>. Also, you can try interactive lessons for CLion by selecting 'Help | Learn IDE Features' from the main menu.
}