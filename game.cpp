//
// Created by denni on 9/28/2026.
//
#include "game.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <iostream>

#include "ui.h"

namespace {

const char kCardNames[kuhn::kNumCards] = {'J', 'Q', 'K'};
const std::string kCardColor[kuhn::kNumCards] = {ui::BLUE, ui::YELLOW, ui::RED};

std::string CardPlain(int card) { return std::string("[ ") + kCardNames[card] + " ]"; }
std::string CardColored(int card) {
    return ui::BOLD + kCardColor[card] + CardPlain(card) + ui::RESET;
}

int Pot(const std::string& history) {
    return 2 + static_cast<int>(std::count_if(history.begin(), history.end(), [](char c) {
               return c == kuhn::kBet || c == kuhn::kCall;
           }));
}

std::string ActionVerb(char action) {
    switch (action) {
        case kuhn::kCheck: return "checks";
        case kuhn::kBet:   return "bets";
        case kuhn::kCall:  return "calls";
        case kuhn::kFold:  return "folds";
    }
    return "?";
}

}  // namespace

Game::Game(const cfr::InfoSetTable& table, unsigned seed) : table_(table), rng_(seed) {}

int Game::BotAction(const kuhn::Deal& deal, const std::string& history) {
    const int player = kuhn::CurrentPlayer(history);
    const cfr::Strategy s =
        table_[player][deal[player]][kuhn::Step(history)].AverageStrategy();
    std::discrete_distribution<int> dist(s.begin(), s.end());
    return dist(rng_);
}

std::string Game::DescribeAction(const std::string& history, int i) const {
    const bool is_human = (i % 2) == human_seat_;
    std::string verb = ActionVerb(history[i]);
    if (is_human) verb.pop_back();  // "You bet", nu "You bets"
    return (is_human ? "You " : "Bot ") + verb;
}

void Game::Render(const kuhn::Deal& deal, const std::string& history, bool reveal,
                  const std::string& message) const {
    using namespace ui;
    const int bot_seat = 1 - human_seat_;

    Clear();
    BoxTop();
    const std::string title = "  KUHN POKER";
    BoxLine(title, BOLD + title + RESET);
    const std::string sub = "  vs. CFR bot (Nash equilibrium strategy)";
    BoxLine(sub, DIM + sub + RESET);
    BoxSep();

    // Status: mana, pozitie, scor
    const std::string pos = human_seat_ == 0 ? "you act first" : "bot acts first";
    // Dupa final, hands_ a fost deja incrementat pentru mana curenta
    const int hand_no = kuhn::IsTerminal(history) ? hands_ : hands_ + 1;
    BoxSplit("  Hand " + std::to_string(hand_no), BOLD + "  Hand " + std::to_string(hand_no) + RESET,
             "(" + pos + ")  ", DIM + "(" + pos + ")" + RESET + "  ");
    const std::string net = Signed(net_chips_);
    const std::string net_col = net_chips_ > 0 ? GREEN : (net_chips_ < 0 ? RED : YELLOW);
    const std::string per_hand = hands_ > 0 ? Fmt(static_cast<double>(net_chips_) / hands_, 3) : "-";
    BoxLine("  Your chips: " + net + "   per hand: " + per_hand,
            "  Your chips: " + net_col + BOLD + net + RESET + "   per hand: " + BOLD + per_hand + RESET);
    BoxSep();

    // Masa: cartile si potul
    const std::string bot_plain = reveal ? CardPlain(deal[bot_seat]) : "[ ? ]";
    const std::string bot_col = reveal ? CardColored(deal[bot_seat]) : DIM + "[ ? ]" + RESET;
    BoxLine("  Bot   " + bot_plain, "  Bot   " + bot_col);
    BoxLine("  You   " + CardPlain(deal[human_seat_]), "  You   " + CardColored(deal[human_seat_]));
    const std::string pot = "  Pot   " + std::to_string(Pot(history)) + " chips";
    BoxLine(pot, "  Pot   " + BOLD + std::to_string(Pot(history)) + RESET + " chips");
    BoxSep();

    // Actiunile din mana curenta
    if (history.empty()) {
        BoxLine("  (no actions yet)", DIM + std::string("  (no actions yet)") + RESET);
    }
    for (int i = 0; i < static_cast<int>(history.size()); ++i) {
        const std::string text = "  " + DescribeAction(history, i);
        const bool is_human = (i % 2) == human_seat_;
        BoxLine(text, (is_human ? CYAN : MAGENTA) + text + RESET);
    }
    BoxSep();

    // Ultimele maini
    std::string last_plain = "  Last:", last_col = "  Last:";
    const int start = std::max(0, static_cast<int>(results_.size()) - 12);
    for (int i = start; i < static_cast<int>(results_.size()); ++i) {
        const int r = results_[i];
        const std::string s = " " + Signed(r);
        last_plain += s;
        last_col += (r > 0 ? GREEN : RED) + s + RESET;
    }
    BoxLine(last_plain, last_col);
    BoxSep();

    // Controale
    std::string controls;
    if (kuhn::IsTerminal(history)) {
        controls = "  [Enter] Next hand   [q] Quit";
    } else if (kuhn::FacingBet(history)) {
        controls = "  [f] Fold   [c] Call   [q] Quit";
    } else {
        controls = "  [k] Check   [b] Bet   [q] Quit";
    }
    BoxLine(controls, DIM + controls + RESET);
    BoxBottom();

    if (!message.empty()) std::cout << "  " << message << "\n";
    std::cout << "  " << BOLD << "> " << RESET << std::flush;
}

bool Game::PlayHand() {
    std::array<int, kuhn::kNumCards> cards{kuhn::Jack, kuhn::Queen, kuhn::King};
    std::shuffle(cards.begin(), cards.end(), rng_);
    const kuhn::Deal deal{cards[0], cards[1]};

    std::string history;
    std::string message;
    std::string input;

    while (!kuhn::IsTerminal(history)) {
        const int player = kuhn::CurrentPlayer(history);
        const auto actions = kuhn::Actions(history);

        if (player != human_seat_) {
            history.push_back(actions[BotAction(deal, history)]);
            continue;
        }

        Render(deal, history, false, message);
        if (!std::getline(std::cin, input)) return false;
        const char c = input.empty() ? '\0' : static_cast<char>(std::tolower(static_cast<unsigned char>(input[0])));
        if (c == 'q') return false;

        if (c == actions[0] || c == actions[1]) {
            history.push_back(c);
            message.clear();
        } else {
            message = ui::YELLOW + std::string("Press ") + actions[0] + " or " + actions[1] + "." + ui::RESET;
        }
    }

    // Rezultatul mainii
    const int p1_payoff = kuhn::Payoff(deal, history);
    const int result = human_seat_ == 0 ? p1_payoff : -p1_payoff;
    const bool showdown = history.back() != kuhn::kFold;

    ++hands_;
    net_chips_ += result;
    if (result > 0) ++hands_won_;
    results_.push_back(result);

    std::string outcome = result > 0 ? ui::GREEN + ui::BOLD + "You win " + std::to_string(result)
                                     : ui::RED + ui::BOLD + "You lose " + std::to_string(-result);
    outcome += result == 1 || result == -1 ? " chip" : " chips";
    outcome += ui::RESET + (showdown ? "  (showdown)" : "  (fold)");

    Render(deal, history, showdown, outcome);
    if (!std::getline(std::cin, input)) return false;
    if (!input.empty() && std::tolower(static_cast<unsigned char>(input[0])) == 'q') return false;

    human_seat_ = 1 - human_seat_;  // alternam pozitiile
    return true;
}

void Game::RenderSummary() const {
    using namespace ui;
    Clear();
    BoxTop();
    const std::string t = "  GAME OVER";
    BoxLine(t, BOLD + t + RESET);
    BoxSep();
    BoxLine("  Hands played: " + std::to_string(hands_));
    BoxLine("  Hands won:    " + std::to_string(hands_won_));
    BoxLine("  Net chips:    " + Signed(net_chips_));
    if (hands_ > 0) {
        BoxLine("  Per hand:     " + Fmt(static_cast<double>(net_chips_) / hands_, 3));
    }
    BoxSep();
    const std::string note1 = "  With alternating seats, the equilibrium bot";
    const std::string note2 = "  cannot lose in expectation over many hands.";
    BoxLine(note1, DIM + note1 + RESET);
    BoxLine(note2, DIM + note2 + RESET);
    BoxBottom();
}

void Game::Run() {
    while (PlayHand()) {
    }
    RenderSummary();
}