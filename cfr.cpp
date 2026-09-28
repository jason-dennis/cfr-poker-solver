//
// Created by denni on 9/28/2026.
//
#include "cfr.h"
#include <algorithm>

namespace cfr {

// ── InfoSet ────────────────────────────────────────────────────

Strategy InfoSet::CurrentStrategy() const {
    Strategy strategy{};
    double total = 0.0;
    for (int a = 0; a < kuhn::kNumActions; ++a) {
        strategy[a] = std::max(0.0, regret_sum[a]);
        total += strategy[a];
    }
    for (int a = 0; a < kuhn::kNumActions; ++a) {
        strategy[a] = total > 0.0 ? strategy[a] / total : 1.0 / kuhn::kNumActions;
    }
    return strategy;
}

Strategy InfoSet::AverageStrategy() const {
    Strategy strategy{};
    double total = 0.0;
    for (double s : strategy_sum) total += s;
    for (int a = 0; a < kuhn::kNumActions; ++a) {
        strategy[a] = total > 0.0 ? strategy_sum[a] / total : 1.0 / kuhn::kNumActions;
    }
    return strategy;
}

// ── KuhnSolver ─────────────────────────────────────────────────

KuhnSolver::KuhnSolver(unsigned seed) : rng_(seed) {}

const InfoSetTable& KuhnSolver::Table() const {
    return table_;
}

InfoSet& KuhnSolver::NodeFor(const kuhn::Deal& deal, const std::string& history) {
    const int player = kuhn::CurrentPlayer(history);
    return table_[player][deal[player]][kuhn::Step(history)];
}

const InfoSet& KuhnSolver::NodeFor(const kuhn::Deal& deal, const std::string& history) const {
    const int player = kuhn::CurrentPlayer(history);
    return table_[player][deal[player]][kuhn::Step(history)];
}

double KuhnSolver::Traverse(const kuhn::Deal& deal, std::string& history,
                            std::array<double, kuhn::kNumPlayers> reach) {
    if (kuhn::IsTerminal(history)) {
        return kuhn::Payoff(deal, history);
    }

    const int player = kuhn::CurrentPlayer(history);
    const int opponent = 1 - player;
    const double sign = player == 0 ? 1.0 : -1.0;

    InfoSet& node = NodeFor(deal, history);
    const Strategy strategy = node.CurrentStrategy();
    const auto actions = kuhn::Actions(history);

    Strategy action_values{};
    double node_value = 0.0;
    for (int a = 0; a < kuhn::kNumActions; ++a) {
        auto child_reach = reach;
        child_reach[player] *= strategy[a];

        history.push_back(actions[a]);
        action_values[a] = sign * Traverse(deal, history, child_reach);
        history.pop_back();

        node_value += strategy[a] * action_values[a];
    }

    for (int a = 0; a < kuhn::kNumActions; ++a) {
        node.regret_sum[a] += reach[opponent] * (action_values[a] - node_value);
        node.strategy_sum[a] += reach[player] * strategy[a];
    }

    return sign * node_value;
}

double KuhnSolver::Evaluate(const kuhn::Deal& deal, std::string& history) const {
    if (kuhn::IsTerminal(history)) {
        return kuhn::Payoff(deal, history);
    }

    const Strategy strategy = NodeFor(deal, history).AverageStrategy();
    const auto actions = kuhn::Actions(history);

    double value = 0.0;
    for (int a = 0; a < kuhn::kNumActions; ++a) {
        history.push_back(actions[a]);
        value += strategy[a] * Evaluate(deal, history);
        history.pop_back();
    }
    return value;
}

double KuhnSolver::Train(int iterations) {
    std::array<int, kuhn::kNumCards> cards{kuhn::Jack, kuhn::Queen, kuhn::King};
    double total = 0.0;

    for (int i = 0; i < iterations; ++i) {
        std::shuffle(cards.begin(), cards.end(), rng_);
        const kuhn::Deal deal{cards[0], cards[1]};
        std::string history;
        total += Traverse(deal, history, {1.0, 1.0});
    }
    return iterations > 0 ? total / iterations : 0.0;
}

double KuhnSolver::GameValue() const {
    double total = 0.0;
    int deals = 0;
    for (int c1 = 0; c1 < kuhn::kNumCards; ++c1) {
        for (int c2 = 0; c2 < kuhn::kNumCards; ++c2) {
            if (c1 == c2) continue;
            std::string history;
            total += Evaluate({c1, c2}, history);
            ++deals;
        }
    }
    return total / deals;
}

}