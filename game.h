//
// Created by denni on 9/28/2026.
//

#ifndef KUHN_POKER_GAME_H
#define KUHN_POKER_GAME_H
#include <random>
#include <string>
#include <vector>

#include "cfr.h"
#include "kuhn.h"

class Game {
public:
    Game(const cfr::InfoSetTable& table, unsigned seed = std::random_device{}());

    // Bucla principala. Se termina cand jucatorul apasa q.
    void Run();

private:
    // Joaca o mana. Intoarce false daca jucatorul a iesit.
    bool PlayHand();

    // Botul alege o actiune (index 0/1) din strategia medie.
    int BotAction(const kuhn::Deal& deal, const std::string& history);

    void Render(const kuhn::Deal& deal, const std::string& history, bool reveal,
                const std::string& message) const;
    void RenderSummary() const;

    // Numele actiunii de la pozitia i din istoric, ex. "You bet", "Bot folds".
    std::string DescribeAction(const std::string& history, int i) const;

    const cfr::InfoSetTable& table_;
    std::mt19937 rng_;

    int human_seat_ = 1;  // 0 = tu esti P1, 1 = tu esti P2 (alterneaza)
    int hands_ = 0;
    int net_chips_ = 0;
    int hands_won_ = 0;
    std::vector<int> results_;  // castigul tau pe fiecare mana
};
#endif //KUHN_POKER_GAME_H