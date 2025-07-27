#include "winning_hand.hpp"

const QString MELDED_CHAR = "#";

QString groupTypeToString(const ClassicGroupType &type) {
    if (type == ClassicGroupType::CHII) {
        return "C";
    } else if (type == ClassicGroupType::PON) {
        return "P";
    } else {
        return "K";
    }
}

QString ClassicGroup::toString() const {
    return groupTypeToString(type) + tile.toString() +
           (melded ? MELDED_CHAR : "");
}
bool ClassicGroup::isSimple() const {
    return (!tile.isHonor()) && (tile.value() > 1) &&
           ((type != ClassicGroupType::CHII) || (tile.value() < 7));
}

QString duoToString(const Tile &tile) { return "D" + tile.toString(); }

QString ClassicHand::toString() const {
    QString result;
    for (const auto &group : groups) {
        result += group.toString() + "-";
    }
    result += duoToString(duo_tile);
    return result;
}

HandScore::HandScore() : fu_(20), fan_(0) {}

int HandScore::totalFu() const { return fu_; }
int HandScore::totalFan() const { return fan_; }
const QVector<ValueDetail> &HandScore::fuDetails() const { return fu_details_; }
const QVector<ValueDetail> &HandScore::yakus() const { return yakus_; }

void HandScore::addFu(int fu, const QString &detail) {
    fu_ += fu;
    fu_details_.append(ValueDetail{fu, detail});
}
void HandScore::addYaku(int fan, const QString &detail) {
    fan_ += fan;
    yakus_.append(ValueDetail{fan, detail});
}
void HandScore::addBetterYaku(int fan, const QString &detail) {
    fan_ += fan;
    yakus_.append(ValueDetail{fan, "<b style=\"color: rgba(8, 95, 150, 1);\">" +
                                       detail + "</b>"});
}
void HandScore::addYakuman(const QString &detail, bool doubled) {
    int value = YAKUMAN;
    if (doubled)
        value *= 2;
    fan_ += value;
    yakus_.append(
        ValueDetail{value, "<b style =\"color: rgba(17, 146, 60, 1);\">" +
                               detail + "</b>"});
}

QString HandScore::toString() const {
    QString text = "<hr><p><b>Fu:</b> " + QString::number(totalFu()) +
                   (fu_details_.size() > 0 ? " = 20" : "");
    for (const auto &fu_detail : fu_details_) {
        text += " + " + QString::number(fu_detail.value);
    }

    text += "<br><b>Fan:</b> " + QString::number(totalFan()) +
            "</p>\n<b>Yakus:</b>\n<ul>\n";
    // Add yaku names
    for (const auto &yaku : yakus_) {
        text += "  <li>" + yaku.detail + " (+" + QString::number(yaku.value) +
                ")" + "</li>\n";
    }
    text += "</ul>";
    return text;
}

WinningHand::WinningHand(const ClassicHand &classic_hand,
                         const Tile &dominant_wind, const Tile &player_wind,
                         bool riichi, bool ippatsu, bool ron, int total_doras)
    : type_(HandType::CLASSIC), hand_(classic_hand), riichi_(riichi),
      ippatsu_(ippatsu), ron_(ron), total_doras_(total_doras),
      dominant_wind_(dominant_wind), player_wind_(player_wind) {}
WinningHand::WinningHand(Tile seven_pairs[7], const Tile &dominant_wind,
                         const Tile &player_wind, bool riichi, bool ippatsu,
                         bool ron, int total_doras)
    : type_(HandType::PAIRS), hand_(seven_pairs), riichi_(riichi),
      ippatsu_(ippatsu), ron_(ron), total_doras_(total_doras),
      dominant_wind_(dominant_wind), player_wind_(player_wind) {}
WinningHand::WinningHand(Tile duo_orphans_hand, const Tile &dominant_wind,
                         const Tile &player_wind, bool riichi, bool ippatsu,
                         bool ron, int total_doras)
    : type_(HandType::ORPHANS), hand_(duo_orphans_hand), riichi_(riichi),
      ippatsu_(ippatsu), ron_(ron), total_doras_(total_doras),
      dominant_wind_(dominant_wind), player_wind_(player_wind) {}

HandType WinningHand::type() const { return type_; }
HandTiles WinningHand::hand() const { return hand_; }
bool WinningHand::isRiichi() const { return riichi_; }
bool WinningHand::isIppatsu() const { return ippatsu_; }
bool WinningHand::isRon() const { return ron_; }
bool WinningHand::isTsumo() const { return !ron_; }
bool WinningHand::isClosed() const {
    return (type_ != HandType::CLASSIC) ||
           (!hand_.classic_hand.groups[0].melded &&
            !hand_.classic_hand.groups[1].melded &&
            !hand_.classic_hand.groups[2].melded &&
            !hand_.classic_hand.groups[3].melded);
}
int WinningHand::totalDoras() const { return total_doras_; }

QString WinningHand::toString() const {
    QString result;
    switch (type_) {
    case HandType::CLASSIC:
        return hand_.classic_hand.toString();
    case HandType::PAIRS:
        for (int i = 0; i < 7; i++) {
            result += hand_.seven_pairs_hand[i].toString() + "-";
        }
        return "7P" + result;
    case HandType::ORPHANS:
        return "13O" + hand_.duo_orphans_hand.toString();
    }
    return "Unknown";
}

/* Scoring methods */
ValidityStatus WinningHand::checkValid() const {
    // Ippatsu requires riichi
    if (ippatsu_ && !riichi_)
        return ValidityStatus(false, "Ippatsu requires riichi");
    // Riichi requires no melds
    if (riichi_ && type_ == HandType::CLASSIC) {
        for (int i = 0; i < 4; i++) {
            if (hand_.classic_hand.groups[i].melded)
                return ValidityStatus(false, "Riichi requires no melds");
        }
    }
    // Orphans requires orphan duo tile
    if (type_ == HandType::ORPHANS && !hand_.duo_orphans_hand.isOrphan()) {
        return ValidityStatus(false, "Orphans requires orphan duo");
    }
    // Chiis cannot have tiles with value > 7
    if (type_ == HandType::CLASSIC) {
        for (int i = 0; i < 4; i++) {
            if (hand_.classic_hand.groups[i].type == ClassicGroupType::CHII &&
                hand_.classic_hand.groups[i].tile.value() > 7) {
                return ValidityStatus(false,
                                      "Chiis cannot have tiles with value > 7");
            }
        }
    }

    return ValidityStatus(true, "");
}

HandScore WinningHand::computeScore() const {
    HandScore score;

    /* Compute fu */
    if (type_ == HandType::PAIRS) {
        score.addFu(5, "Seven Pairs");
    } else if (type_ == HandType::ORPHANS) {
        // TODO
    } else { // Classic
        // Handle pair
        if (hand_.classic_hand.duo_tile.isDragon()) {
            score.addFu(2, "Dragon Pair");
        }
        if (hand_.classic_hand.duo_tile.isWind()) {
            if (hand_.classic_hand.duo_tile == dominant_wind_) {
                score.addFu(2, "Dominant wind pair");
            } else if (hand_.classic_hand.duo_tile == player_wind_) {
                score.addFu(2, "Player's wind pair");
            }
        }
        // Handle pons (including kans)
        for (int i = 0; i < 4; i++) {
            if (hand_.classic_hand.groups[i].type == ClassicGroupType::CHII) {
                continue;
            }
            int value = 2;
            QString expl;
            if (!hand_.classic_hand.groups[i].melded) {
                value *= 2;
                expl += "Concealed ";
            } else {
                expl += "Melded ";
            }
            if (hand_.classic_hand.groups[i].tile.isOrphan()) {
                value *= 2;
                expl += "major ";
            } else {
                expl += "simple ";
            }
            if (hand_.classic_hand.groups[i].type == ClassicGroupType::KAN) {
                value *= 4;
                expl += "kan";
            } else {
                expl += "pon";
            }
            score.addFu(value, expl);
        }
    }

    if (type_ != HandType::PAIRS && isTsumo() && score.totalFu() > 20) {
        score.addFu(2, "Tsumo not Pinfu");
    }

    // Handle pinfu
    if (type_ == HandType::CLASSIC && score.totalFu() == 20) {
        score.addYaku(1, "Pinfu");
    }
    if (type_ != HandType::PAIRS && isClosed() && isRon()) {
        score.addFu(10, "Closed Hand won by ron");
    }

    /* Compute fans */
    if (isRiichi()) {
        score.addYaku(1, "Riichi");
    }
    if (isIppatsu()) {
        score.addYaku(1, "Ippatsu");
    }
    if (isClosed() && isTsumo()) {
        score.addYaku(1, "Fully concealed hand");
    }
    if (type_ == HandType::ORPHANS) {
        score.addYakuman("Thirteen Orphans", true);
    } else if (type_ == HandType::PAIRS) {
        score.addYaku(2, "Seven pairs");
    }
    // TODO Handle twice double chii
    else if (type_ == HandType::CLASSIC) {
        int n_simple = 0, n_chii = 0, n_concealed_pon = 0, n_pon = 0, n_kan = 0,
            n_dragon_pon = 0, n_wind_pon = 0, n_group_with_terminal = 0;
        for (const auto &group : hand_.classic_hand.groups) {
            if (group.isSimple()) {
                n_simple += 1;
            }
            if (group.type == ClassicGroupType::CHII) {
                n_chii += 1;
                if (!group.isSimple()) {
                    n_group_with_terminal += 1;
                }
            } else { // Pon or Kan
                n_pon += 1;
                if (group.type == ClassicGroupType::KAN) {
                    n_kan += 1;
                }
                if (!group.melded) {
                    n_concealed_pon += 1;
                }
                if (group.tile.isTerminal()) {
                    n_group_with_terminal += 1;
                }
                if (group.tile.isDragon()) {
                    score.addYaku(1, "Dragon pon");
                    n_dragon_pon += 1;
                }
                if (group.tile.isWind()) {
                    n_wind_pon += 1;
                    if (group.tile == dominant_wind_) {
                        score.addYaku(1, "Prevailing wind pon");
                    }
                    if (group.tile == player_wind_) {
                        score.addYaku(1, "Player's wind pon");
                    }
                }
            }
        }
        int n_group_with_orphan =
            n_group_with_terminal + n_dragon_pon + n_wind_pon;
        if (n_simple == 4 && !hand_.classic_hand.duo_tile.isOrphan()) {
            score.addYaku(1, "All simple");
        }
        if (n_pon == 4) {
            score.addYaku(2, "All pon");
        }
        if (n_concealed_pon >= 3) {
            score.addYaku(2, "Three concealed pon");
        }
        if (n_kan == 3) {
            score.addYaku(2, "Three kan");
        } else if (n_kan == 4) {
            score.addYakuman("Four kan");
        }
        if (n_dragon_pon == 3) {
            score.addYakuman("Big Three Dragons");
        } else if (n_dragon_pon == 2 &&
                   hand_.classic_hand.duo_tile.isDragon()) {
            score.addBetterYaku(4, "Little Three Dragons");
        }
        if (n_wind_pon == 4) {
            score.addYakuman("Big Four Winds", true);
        } else if (n_wind_pon == 3 && hand_.classic_hand.duo_tile.isWind()) {
            score.addYakuman("Little Four Winds");
        }
        // Terminal (and honors) yaku
        if (n_group_with_orphan == 4 &&
            hand_.classic_hand.duo_tile.isOrphan()) {
            if (n_dragon_pon + n_wind_pon == 4 &&
                hand_.classic_hand.duo_tile.isHonor()) {
                score.addYakuman("All Honors Hand");
            } else if (n_group_with_terminal == 4 &&
                       hand_.classic_hand.duo_tile.isTerminal()) {
                if (n_chii == 0) {
                    score.addYakuman("All Terminals Hand");
                } else {
                    score.addYaku(2 + (isClosed() ? 1 : 0),
                                  (isClosed() ? "Closed" : "Open") +
                                      QString(" Pure Outside Hand"));
                }
            } else if (n_chii == 0) {
                score.addYaku(2, "All Terminals and Honors Hand");
            } else {
                score.addYaku(1 + (isClosed() ? 1 : 0),
                              (isClosed() ? "Closed" : "Open") +
                                  QString(" Mixed Outside Hand"));
            }
        }
    }
    if (total_doras_ > 0) {
        score.addYaku(total_doras_, "Doras");
    }

    return score;
}
