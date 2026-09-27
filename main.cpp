#include <algorithm>
#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <cassert>
#include <array>
#include <iomanip>
using Table4D = std::vector<std::vector<std::vector<std::vector<double>>>>;
void PrintStrategy(const Table4D& strategy_sum) {
    const char card_names[3] = {'J', 'Q', 'K'};
    const std::string histories[2][2] = {{"-", "kb"}, {"k", "b"}};

    std::cout << std::left
              << std::setw(8)  << "Player"
              << std::setw(6)  << "Card"
              << std::setw(10) << "History"
              << "Average strategy\n"
              << std::string(50, '-') << '\n';

    for (int player = 0; player < 2; ++player) {
        for (int step = 0; step < 2; ++step) {
            for (int card = 0; card < 3; ++card) {
                const auto& s = strategy_sum[player][card][step];
                double total = s[0] + s[1];
                double p0 = total > 0 ? s[0] / total : 0.5;
                double p1 = 1.0 - p0;

                // P1 la pasul 1 si P2 la pasul 1 raspund la un bet
                bool facing_bet = (step == 1);
                std::string a0 = facing_bet ? "fold" : "check";
                std::string a1 = facing_bet ? "call" : "bet";

                std::cout << std::left << std::fixed << std::setprecision(3)
                          << std::setw(8)  << (player == 0 ? "P1" : "P2")
                          << std::setw(6)  << card_names[card]
                          << std::setw(10) << histories[player][step]
                          << std::setw(6)  << a0 << std::setw(8) << p0
                          << std::setw(5)  << a1 << p1 << '\n';
            }
        }
        std::cout << '\n';
    }
}
int WhosTurn(const std::string& history) {
    return (int)history.size() % 2;
}
// k - check, b - bet, c - call, f - fold
// possible history -> kk, kbc, kbf, bc, bf
bool IsOver(const std::string& history) {
    if (history == "kk" or history == "kbc" or history == "kbf" or history == "bc" or history == "bf") {
        return true;
    }
    return false;
}
int CalcReward(std::array<int,2>& player_card, const std::string& history) {
    assert(IsOver(history) == true);
    if (player_card[0] > player_card[1]) {
        if (history == "kk" or history == "bf") {
            return 1;
        }
        if (history == "kbc" or history == "bc") {
            return 2;
        }
        return -1;
    }
    if (history == "kk" or history == "kbf") {
        return -1;
    }
    if (history == "kbc" or history == "bc") {
        return -2;
    }
    return 1;
}
double PlayRound(double reach_player_one, double reach_player_two, std::array<int,2>& player_card, std::string& history, std::vector<std::vector<std::vector<std::vector<double>>>>&strategy,
                std::vector<std::vector<std::vector<std::vector<double>>>>& regret, std::vector<std::vector<std::vector<std::vector<double>>>>& average_strategy) {
    if (IsOver(history)){
        return CalcReward(player_card, history);
    }
    const auto player = WhosTurn(history);
    double reward_action_one;
    double reward_action_two;

    auto UseAction =[&](char action) {
        history.push_back(action);
        double reward = PlayRound(reach_player_one, reach_player_two, player_card, history, strategy, regret, average_strategy);
        history.pop_back();
        return reward;
    };
    auto UpdateStrategy=[&](int step, double reach_one, double reach_two, double reward, double reward_action_one, double reward_action_two) {
        regret[player][player_card[player]][step][0] += (reward_action_one - reward) * reach_two;
        regret[player][player_card[player]][step][1] += (reward_action_two - reward) * reach_two;

        average_strategy[player][player_card[player]][step][0] +=  strategy[player][player_card[player]][step][0] * reach_one;
        average_strategy[player][player_card[player]][step][1] += strategy[player][player_card[player]][step][1] * reach_one;

        double totals = 0;
        if (regret[player][player_card[player]][step][0] > 0)
            totals += regret[player][player_card[player]][step][0];
        if (regret[player][player_card[player]][step][1] > 0)
            totals += regret[player][player_card[player]][step][1];
        if (totals == 0) {
            strategy[player][player_card[player]][step][0] = 0.5;
            strategy[player][player_card[player]][step][1] = 0.5;
        }
        else {
            if (regret[player][player_card[player]][step][0] > 0)
                strategy[player][player_card[player]][step][0] = (double) regret[player][player_card[player]][step][0] / totals * 1.0;
            else
                strategy[player][player_card[player]][step][0] = 0.0;
            if (regret[player][player_card[player]][step][1] > 0)
                strategy[player][player_card[player]][step][1] = (double) regret[player][player_card[player]][step][1] / totals * 1.0;
            else
                strategy[player][player_card[player]][step][1] = 0.0;
        }

    };

    if (player == 0) {
        if (history.empty()) {
            { // check
                double reach = reach_player_one;
                reach_player_one *= strategy[player][player_card[player]][0][0];
                reward_action_one = UseAction('k');
                reach_player_one = reach;
            }
            { // bet
                double reach = reach_player_one;
                reach_player_one *= strategy[player][player_card[player]][0][1];
                reward_action_two = UseAction('b');
                reach_player_one = reach;
            }
            double reward = reward_action_one * strategy[player][player_card[player]][0][0] + reward_action_two *
                      strategy[player][player_card[player]][0][1];
            UpdateStrategy(0, reach_player_one, reach_player_two, reward, reward_action_one, reward_action_two);
            return reward;
        }
        else {
            { // fold
                double reach = reach_player_one;
                reach_player_one *= strategy[player][player_card[player]][1][0];
                reward_action_one = UseAction('f');
                reach_player_one = reach;
            }
            { // call
                double reach = reach_player_one;
                reach_player_one *= strategy[player][player_card[player]][1][1];
                reward_action_two = UseAction('c');
                reach_player_one = reach;
            }
            double reward = reward_action_one * strategy[player][player_card[player]][1][0] + reward_action_two *
                      strategy[player][player_card[player]][1][1];
            UpdateStrategy(1, reach_player_one, reach_player_two, reward, reward_action_one, reward_action_two);
            return reward;
        }
    }
    else {
        if (history.back() == 'k') {
            { // check
                double reach = reach_player_two;
                reach_player_two *= strategy[player][player_card[player]][0][0];
                reward_action_one = UseAction('k')* -1.0;
                reach_player_two = reach;
            }
            { // bet
                double reach = reach_player_two;
                reach_player_two *= strategy[player][player_card[player]][0][1];
                reward_action_two = UseAction('b')* -1.0;
                reach_player_two = reach;
            }
            double reward = reward_action_one * strategy[player][player_card[player]][0][0] + reward_action_two *
                      strategy[player][player_card[player]][0][1];
            UpdateStrategy(0, reach_player_two, reach_player_one, reward, reward_action_one, reward_action_two);
            return reward * -1.0;
        }
        else {
            { // fold
                double reach = reach_player_two;
                reach_player_two *= strategy[player][player_card[player]][1][0];
                reward_action_one = UseAction('f')* -1.0;
                reach_player_two = reach;
            }
            { // call
                double reach = reach_player_two;
                reach_player_two *= strategy[player][player_card[player]][1][1];
                reward_action_two = UseAction('c') * -1.0;
                reach_player_two = reach;
            }
            double reward = reward_action_one * strategy[player][player_card[player]][1][0] + reward_action_two *
                                 strategy[player][player_card[player]][1][1];
            UpdateStrategy(1, reach_player_two, reach_player_one, reward, reward_action_one, reward_action_two);
            return reward * -1.0;
        }
    }
}


double TrainingRound(std::vector<std::vector<std::vector<std::vector<double>>>>&strategy, std::vector<std::vector<std::vector<std::vector<double>>>>&regret, std::vector<std::vector<std::vector<std::vector<double>>>>& average_strategy) {
    std::vector<int> cards = {0, 1 ,2};
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(cards.begin(),cards.end(), gen);
    std::array<int,2> player_card{{cards[0], cards[1]}};
    std::string history = "";
    double reach_player_one = 1.0;
    double reach_player_two = 1.0;
    return PlayRound(reach_player_one, reach_player_two, player_card, history, strategy, regret, average_strategy);

}
void TrainingBot() {
    // strategy[0/1][0/1/2][0/1][0/1] - > strategy[i][j][k][l] -> strategy for player i with card j at information set k and action l
    std::vector<std::vector<std::vector<std::vector<double>>>>strategy(2,std::vector<std::vector<std::vector<double>>>(3,std::vector<std::vector<double>>(2,std::vector<double>(2,0.5))));
    std::vector<std::vector<std::vector<std::vector<double>>>>average_strategy(2,std::vector<std::vector<std::vector<double>>>(3,std::vector<std::vector<double>>(2,std::vector<double>(2,0.0))));
    std::vector<std::vector<std::vector<std::vector<double>>>>regret(2,std::vector<std::vector<std::vector<double>>>(3,std::vector<std::vector<double>>(2,std::vector<double>(2))));
    double result = 0.0;
    for (int i{}; i < 100000; ++i) {
        result += TrainingRound(strategy, regret, average_strategy);
    }
    PrintStrategy(average_strategy);

    result = (double) result / 100000.0;
    std::cout<<'\n' << result;

}
void Play() {
    std::vector<int> cards = {0, 1 ,2};
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(cards.begin(),cards.end(), gen);
}

int main() {
    TrainingBot();
}