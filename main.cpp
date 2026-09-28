#include <string>

#include "cfr.h"
#include "game.h"
#include "print.h"
#include "ui.h"

int main() {
    constexpr int kIterations = 100000;

    cfr::KuhnSolver solver;
    solver.Train(kIterations);

    PrintStrategy(solver.Table());
    PrintGameValue(solver.GameValue());

    ui::EnableAnsi();
    Game game(solver.Table());
    game.Run();
}