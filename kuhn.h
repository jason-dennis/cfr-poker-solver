//
// Created by denni on 9/27/2026.
//

#ifndef KUHN_POKER_KUHN_H
#define KUHN_POKER_KUHN_H
#include <string>
#include <array>
namespace kuhn {

    inline constexpr int kNumPlayers = 2;
    inline constexpr int kNumCards   = 3;
    inline constexpr int kNumSteps   = 2;  // puncte de decizie per jucator
    inline constexpr int kNumActions = 2;  // index 0 = pasiva, 1 = agresiva

    enum Card : int { Jack = 0, Queen = 1, King = 2 };

    inline constexpr char kCheck = 'k';
    inline constexpr char kBet   = 'b';
    inline constexpr char kCall  = 'c';
    inline constexpr char kFold  = 'f';

    // deal[p] = cartea jucatorului p (0 = P1, 1 = P2)
    using Deal = std::array<int, kNumPlayers>;

    // Jocul s-a terminat? (kk, kbc, kbf, bc, bf)
    bool IsTerminal(const std::string& history);

    // Cine e la mutare: 0 = P1, 1 = P2. Doar pentru istorice neterminale.
    int CurrentPlayer(const std::string& history);

    // Jucatorul la mutare are un bet in fata?
    bool FacingBet(const std::string& history);

    // Pasul de decizie al jucatorului la mutare:
    //   0 = prima decizie (P1 la "", P2 dupa "k")
    //   1 = raspuns la bet (P1 dupa "kb", P2 dupa "b")
    int Step(const std::string& history);

    // Cele doua actiuni posibile: [0] pasiva (check/fold), [1] agresiva (bet/call)
    std::array<char, kNumActions> Actions(const std::string& history);

    // Castigul pentru P1. Doar pentru istorice terminale.
    int Payoff(const Deal& deal, const std::string& history);

}  // namespace kuhn

#endif //KUHN_POKER_KUHN_H