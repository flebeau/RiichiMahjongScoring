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

QString duoToString(const Tile &tile) { return "D" + tile.toString(); }

QString ClassicHand::toString() const {
    QString result;
    for (const auto &group : groups) {
        result += group.toString() + "-";
    }
    result += duoToString(duo_tile);
    return result;
}

WinningHand::WinningHand(const ClassicHand &classic_hand, bool riichi,
                         bool ippatsu, bool ron, int total_doras)
    : type_(HandType::CLASSIC), hand_(classic_hand), riichi_(riichi),
      ippatsu_(ippatsu), ron_(ron), total_doras_(total_doras) {}
WinningHand::WinningHand(Tile seven_pairs[7], bool riichi, bool ippatsu,
                         bool ron, int total_doras)
    : type_(HandType::PAIRS), hand_(seven_pairs), riichi_(riichi),
      ippatsu_(ippatsu), ron_(ron), total_doras_(total_doras) {}
WinningHand::WinningHand(Tile duo_orphans_hand, bool riichi, bool ippatsu,
                         bool ron, int total_doras)
    : type_(HandType::ORPHANS), hand_(duo_orphans_hand), riichi_(riichi),
      ippatsu_(ippatsu), ron_(ron), total_doras_(total_doras) {}

HandType WinningHand::type() const { return type_; }
HandTiles WinningHand::hand() const { return hand_; }
bool WinningHand::isRiichi() const { return riichi_; }
bool WinningHand::isIppatsu() const { return ippatsu_; }
bool WinningHand::isRon() const { return ron_; }
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
    score.fu = 20;
    score.fan = 0;

    // TODO: Compute score

    return score;
}
