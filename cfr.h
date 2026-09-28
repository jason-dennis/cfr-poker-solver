//
// Created by denni on 9/27/2026.
//

#ifndef KUHN_POKER_CFR_H
#define KUHN_POKER_CFR_H
#include <array>
#include <random>
#include <string>

#include "kuhn.h"

namespace cfr {

    using Strategy = std::array<double, kuhn::kNumActions>;

    struct InfoSet {
        Strategy regret_sum{};
        Strategy strategy_sum{};

        Strategy CurrentStrategy() const;
        Strategy AverageStrategy() const;
    };

    // table[player][card][step]
    using InfoSetTable =
        std::array<std::array<std::array<InfoSet, kuhn::kNumSteps>, kuhn::kNumCards>, kuhn::kNumPlayers>;

    class KuhnSolver {
    public:
        explicit KuhnSolver(unsigned seed = std::random_device{}());

        double Train(int iterations);

        double GameValue() const;

        const InfoSetTable& Table() const;

    private:
        // O parcurgere CFR. Intoarce valoarea nodului din perspectiva lui P1.
        // reach[p] = probabilitatea ca jucatorul p sa ajunga in acest nod.
        double Traverse(const kuhn::Deal& deal, std::string& history,
                        std::array<double, kuhn::kNumPlayers> reach);

        // Parcurgere fara actualizari, cu strategia medie. Valoare pentru P1.
        double Evaluate(const kuhn::Deal& deal, std::string& history) const;

        // Information set-ul jucatorului la mutare, pentru aceasta impartire si acest istoric.
        InfoSet& NodeFor(const kuhn::Deal& deal, const std::string& history);
        const InfoSet& NodeFor(const kuhn::Deal& deal, const std::string& history) const;

        InfoSetTable table_{};
        std::mt19937 rng_;
    };

}
#endif //KUHN_POKER_CFR_H