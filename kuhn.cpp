//
// Created by denni on 9/27/2026.
//
#include "kuhn.h"
#include <cassert>

namespace kuhn {
    bool IsTerminal(const std::string &history) {
        return history == "kk" || history == "kbc" || history == "kbf" || history == "bc" || history == "bf";
    }

    int CurrentPlayer(const std::string &history) {
        return static_cast<int>(history.size() % 2);
    }

    bool FacingBet(const std::string &history) {
        if (history.empty()) return false;
        return history.back() == kBet;
    }

    int Step(const std::string &history) {
        return static_cast<int>(FacingBet(history));
    }

    std::array<char, kNumActions> Actions(const std::string &history) {
        if (!FacingBet(history)) {
            return {kCheck, kBet};
        }
        return {kFold, kCall};
    }

    int Payoff(const Deal &deal, const std::string &history) {
        assert(IsTerminal(history));
        if (history.back() == kFold) {
            const int folder = static_cast<int>(history.size() - 1) % 2;
            return folder == 0 ? -1 : 1;
        }
        const int stake = history.back() == kCall ? 2 : 1;
        return deal[0] > deal[1] ? stake : -stake;
    }



}