#include "turnresult.hpp"
#include <sstream>

TurnResult::TurnResult(int _east_player, int _winner, bool _ron_victory,
                       int _loser, bool _riichi_player_1, bool _riichi_player_2,
                       bool _riichi_player_3, bool _riichi_player_4,
                       int _fu_score, int _fan_score)
    : east_player_(_east_player), winner_(_winner), ron_victory_(_ron_victory),
      loser_(_loser), riichi_player_1(_riichi_player_1),
      riichi_player_2(_riichi_player_2), riichi_player_3(_riichi_player_3),
      riichi_player_4(_riichi_player_4), fu_score_(_fu_score),
      fan_score_(_fan_score) {}

TurnResult::TurnResult(QString *description) : TurnResult() {
    QTextStream in(description);

    int ron, r1, r2, r3, r4;

    in >> east_player_ >> winner_ >> ron >> loser_ >> r1 >> r2 >> r3 >> r4 >>
        fu_score_ >> fan_score_;

    ron_victory_ = static_cast<bool>(ron);
    riichi_player_1 = static_cast<bool>(r1);
    riichi_player_2 = static_cast<bool>(r2);
    riichi_player_3 = static_cast<bool>(r3);
    riichi_player_4 = static_cast<bool>(r4);
}

std::vector<int> TurnResult::computeScoreChange(int n_players) {
    std::vector<int> result = std::vector<int>(n_players, 0);
    int temp = 0;

    /* First handle the case of ron or tsumo victory*/
    if (ron_victory_) {
        // There are two cases: winner is east or not
        if (winner_ == east_player_) {
            // If winner is east, loser pays the amount according to Tabular2
            temp = Tabular2(fu_score_, fan_score_);
            result[loser_] = -temp;
            result[winner_] = temp;
        } else {
            // If winner is not east, loser pays the amount according to
            // Tabular4
            temp = Tabular4(fu_score_, fan_score_);
            result[loser_] = -temp;
            result[winner_] = temp;
        }
    } else { // Tsumo victory
             // There are two cases: winner is east or not
        if (winner_ == east_player_) {
            // If winner is east, each player pays according to Tabular1
            temp = Tabular1(fu_score_, fan_score_);
            for (int i = 0; i < n_players; i++) {
                result[i] -= temp;
            }
            result[winner_] += n_players * temp;
        } else {
            // If winner is not east, east pays according to Tabular1 and other
            // losers pay according to Tabular3
            result[east_player_] -= Tabular1(fu_score_, fan_score_);
            temp = Tabular3(fu_score_, fan_score_);
            for (int i = 0; i < n_players; i++) {
                if (i == east_player_ || i == winner_) {
                    continue;
                }
                result[i] -= temp;
            }
            result[winner_] = (n_players - 2) * temp - result[east_player_];
        }
    }

    /* Handle riichi points */
    unsigned n_riichi_players = 0;
    if (riichi_player_1) {
        result[0] -= 1000;
        n_riichi_players++;
    }
    if (riichi_player_2) {
        result[1] -= 1000;
        n_riichi_players++;
    }
    if (riichi_player_3) {
        result[2] -= 1000;
        n_riichi_players++;
    }
    if (riichi_player_4) {
        result[3] -= 1000;
        n_riichi_players++;
    }
    result[winner_] += n_riichi_players * 1000;

    return result;
}

void TurnResult::writeToTextStream(QTextStream &out) const {
    out << east_player_ << " " << winner_ << " " << ron_victory_ << " "
        << loser_ << " " << riichi_player_1 << " " << riichi_player_2 << " "
        << riichi_player_3 << " " << riichi_player_4 << " " << fu_score_ << " "
        << fan_score_;
}

int TurnResult::Tabular1(int fu, int fan) {
    if (fan == 1) {
        if (fu <= 20) {
            return 0;
        } else if (fu == 25) {
            return 0;
        } else if (fu <= 30) {
            return 500;
        } else if (fu <= 40) {
            return 700;
        } else if (fu <= 50) {
            return 800;
        } else if (fu <= 60) {
            return 1000;
        } else if (fu <= 70) {
            return 1200;
        } else if (fu <= 80) {
            return 1300;
        } else {
            return 1500;
        }
    } else if (fan == 2) {
        if (fu <= 20) {
            return 700;
        } else if (fu == 25) {
            return 0;
        } else if (fu <= 30) {
            return 1000;
        } else if (fu <= 40) {
            return 1300;
        } else if (fu <= 50) {
            return 1600;
        } else if (fu <= 60) {
            return 2000;
        } else if (fu <= 70) {
            return 2300;
        } else if (fu <= 80) {
            return 2600;
        } else {
            return 2900;
        }
    } else if (fan == 3) {
        if (fu <= 20) {
            return 1300;
        } else if (fu == 25) {
            return 1600;
        } else if (fu <= 30) {
            return 2000;
        } else if (fu <= 40) {
            return 2600;
        } else if (fu <= 50) {
            return 3200;
        } else if (fu <= 60) {
            return 3900;
        } else {
            return 4000; // Mangan
        }
    } else if (fan == 4) {
        if (fu <= 20) {
            return 2600;
        } else if (fu == 25) {
            return 3200;
        } else if (fu <= 30) {
            return 3900;
        } else {
            return 4000; // Mangan
        }
    } else if (fan == 5) {
        return 4000; // Mangan
    } else if (fan <= 7) {
        return 6000; // Haneman
    } else if (fan <= 10) {
        return 8000; // Baiman
    } else if (fan <= 12) {
        return 12000;
    } else if (fan <= 25) {
        return 16000;
    } else if (fan <= 38) {
        return 32000;
    } else {
        return 48000;
    }
}

int TurnResult::Tabular2(int fu, int fan) {
    if (fan == 1) {
        if (fu <= 20) {
            return 0;
        } else if (fu == 25) {
            return 0;
        } else if (fu <= 30) {
            return 1500;
        } else if (fu <= 40) {
            return 2000;
        } else if (fu <= 50) {
            return 2400;
        } else if (fu <= 60) {
            return 2900;
        } else if (fu <= 70) {
            return 3400;
        } else if (fu <= 80) {
            return 3900;
        } else {
            return 4400;
        }
    } else if (fan == 2) {
        if (fu <= 20) {
            return 2000;
        } else if (fu == 25) {
            return 2400;
        } else if (fu <= 30) {
            return 2900;
        } else if (fu <= 40) {
            return 3900;
        } else if (fu <= 50) {
            return 4800;
        } else if (fu <= 60) {
            return 5800;
        } else if (fu <= 70) {
            return 6800;
        } else if (fu <= 80) {
            return 7700;
        } else {
            return 8700;
        }
    } else if (fan == 3) {
        if (fu <= 20) {
            return 3900;
        } else if (fu == 25) {
            return 4800;
        } else if (fu <= 30) {
            return 5800;
        } else if (fu <= 40) {
            return 7700;
        } else if (fu <= 50) {
            return 9600;
        } else if (fu <= 60) {
            return 11600;
        } else {
            return 12000; // Mangan
        }
    } else if (fan == 4) {
        if (fu <= 20) {
            return 7700;
        } else if (fu == 25) {
            return 9600;
        } else if (fu <= 30) {
            return 11600;
        } else {
            return 12000; // Mangan
        }
    } else if (fan == 5) {
        return 12000; // Mangan
    } else if (fan <= 7) {
        return 18000; // Haneman
    } else if (fan <= 10) {
        return 24000; // Baiman
    } else if (fan <= 12) {
        return 36000;
    } else if (fan <= 25) {
        return 48000;
    } else if (fan <= 38) {
        return 96000;
    } else {
        return 144000;
    }
}

int TurnResult::Tabular3(int fu, int fan) {
    if (fan == 1) {
        if (fu <= 20) {
            return 0;
        } else if (fu == 25) {
            return 0;
        } else if (fu <= 30) {
            return 300;
        } else if (fu <= 40) {
            return 400;
        } else if (fu <= 50) {
            return 400;
        } else if (fu <= 60) {
            return 500;
        } else if (fu <= 70) {
            return 600;
        } else if (fu <= 80) {
            return 700;
        } else {
            return 800;
        }
    } else if (fan == 2) {
        if (fu <= 20) {
            return 400;
        } else if (fu == 25) {
            return 0;
        } else if (fu <= 30) {
            return 500;
        } else if (fu <= 40) {
            return 700;
        } else if (fu <= 50) {
            return 800;
        } else if (fu <= 60) {
            return 1000;
        } else if (fu <= 70) {
            return 1200;
        } else if (fu <= 80) {
            return 1300;
        } else {
            return 1500;
        }
    } else if (fan == 3) {
        if (fu <= 20) {
            return 700;
        } else if (fu == 25) {
            return 800;
        } else if (fu <= 30) {
            return 1000;
        } else if (fu <= 40) {
            return 1300;
        } else if (fu <= 50) {
            return 1600;
        } else {
            return 2000; // Mangan
        }
    } else if (fan == 4) {
        if (fu <= 20) {
            return 1300;
        } else if (fu == 25) {
            return 1600;
        } else {
            return 2000; // Mangan
        }
    } else if (fan == 5) {
        return 2000; // Mangan
    } else if (fan <= 7) {
        return 3000; // Haneman
    } else if (fan <= 10) {
        return 4000; // Baiman
    } else if (fan <= 12) {
        return 6000;
    } else if (fan <= 25) {
        return 8000;
    } else if (fan <= 38) {
        return 16000;
    } else {
        return 24000;
    }
}

int TurnResult::Tabular4(int fu, int fan) {
    if (fan == 1) {
        if (fu <= 20) {
            return 0;
        } else if (fu == 25) {
            return 0;
        } else if (fu <= 30) {
            return 1000;
        } else if (fu <= 40) {
            return 1300;
        } else if (fu <= 50) {
            return 1600;
        } else if (fu <= 60) {
            return 2000;
        } else if (fu <= 70) {
            return 2300;
        } else if (fu <= 80) {
            return 2600;
        } else {
            return 2900;
        }
    } else if (fan == 2) {
        if (fu <= 20) {
            return 1600;
        } else if (fu == 25) {
            return 1600;
        } else if (fu <= 30) {
            return 2000;
        } else if (fu <= 40) {
            return 3600;
        } else if (fu <= 50) {
            return 3200;
        } else if (fu <= 60) {
            return 3900;
        } else if (fu <= 70) {
            return 4500;
        } else if (fu <= 80) {
            return 5200;
        } else {
            return 5800;
        }
    } else if (fan == 3) {
        if (fu <= 20) {
            return 2800;
        } else if (fu == 25) {
            return 3200;
        } else if (fu <= 30) {
            return 3900;
        } else if (fu <= 40) {
            return 5200;
        } else if (fu <= 50) {
            return 6400;
        } else if (fu <= 60) {
            return 7700;
        } else {
            return 8000; // Mangan
        }
    } else if (fan == 4) {
        if (fu <= 20) {
            return 5200;
        } else if (fu == 25) {
            return 6400;
        } else if (fu <= 30) {
            return 7700;
        } else {
            return 8000; // Mangan
        }
    } else if (fan == 5) {
        return 8000; // Mangan
    } else if (fan <= 7) {
        return 12000; // Haneman
    } else if (fan <= 10) {
        return 16000; // Baiman
    } else if (fan <= 12) {
        return 24000;
    } else if (fan <= 25) {
        return 32000;
    } else if (fan <= 38) {
        return 64000;
    } else {
        return 96000;
    }
}