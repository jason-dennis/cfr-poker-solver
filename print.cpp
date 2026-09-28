//
// Created by denni on 9/28/2026.
//
#include "print.h"

#include <iomanip>
#include <iostream>
#include <string>

void PrintStrategy(const cfr::InfoSetTable& table) {
    const char card_names[kuhn::kNumCards] = {'J', 'Q', 'K'};
    const std::string histories[kuhn::kNumPlayers][kuhn::kNumSteps] = {{"-", "kb"}, {"k", "b"}};

    std::cout << std::left
              << std::setw(8) << "Player"
              << std::setw(6) << "Card"
              << std::setw(10) << "History"
              << "Average strategy\n"
              << std::string(50, '-') << '\n';

    for (int player = 0; player < kuhn::kNumPlayers; ++player) {
        for (int step = 0; step < kuhn::kNumSteps; ++step) {
            for (int card = 0; card < kuhn::kNumCards; ++card) {
                const cfr::Strategy s = table[player][card][step].AverageStrategy();
                const bool facing_bet = step == 1;
                const std::string a0 = facing_bet ? "fold" : "check";
                const std::string a1 = facing_bet ? "call" : "bet";

                std::cout << std::left << std::fixed << std::setprecision(3)
                          << std::setw(8) << (player == 0 ? "P1" : "P2")
                          << std::setw(6) << card_names[card]
                          << std::setw(10) << histories[player][step]
                          << std::setw(6) << a0 << std::setw(8) << s[0]
                          << std::setw(5) << a1 << s[1] << '\n';
            }
        }
        std::cout << '\n';
    }
}

void PrintGameValue(double value) {
    constexpr double kTheoretical = -1.0 / 18.0;
    std::cout << std::fixed << std::setprecision(4)
              << "Game value for P1: " << value
              << "   (theoretical: " << kTheoretical << ")\n";
}