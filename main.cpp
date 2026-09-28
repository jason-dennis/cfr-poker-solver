#include "cfr.h"
#include "print.h"

int main() {
    constexpr int kIterations = 100000;

    cfr::KuhnSolver solver;
    solver.Train(kIterations);

    PrintStrategy(solver.Table());
    PrintGameValue(solver.GameValue());
}