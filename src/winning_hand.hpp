#pragma once

#include "tile.hpp"

enum class HandType { CLASSIC, PAIRS, ORPHANS };

enum class ClassicGroupType { CHII, PON, KAN };

typedef struct ClassicGroup {
    ClassicGroupType type = ClassicGroupType::CHII;
    Tile tile;
    bool melded = false;
    QString toString() const;
} ClassicGroup;

typedef struct ClassicHand {
    ClassicGroup groups[4];
    Tile duo_tile;
    QString toString() const;
} ClassicHand;

union HandTiles {
    ClassicHand classic_hand;
    Tile seven_pairs_hand[7];
    Tile duo_orphans_hand;

    HandTiles(const ClassicHand &classic_hand) : classic_hand(classic_hand) {}
    HandTiles(Tile seven_pairs[7])
        : seven_pairs_hand{seven_pairs[0], seven_pairs[1], seven_pairs[2],
                           seven_pairs[3], seven_pairs[4], seven_pairs[5],
                           seven_pairs[6]} {}
    HandTiles(Tile duo_orphans_hand) : duo_orphans_hand(duo_orphans_hand) {}
};

typedef struct ValidityStatus {
    bool valid;
    QString message;
    ValidityStatus(bool valid_in = true, QString message_in = "")
        : valid(valid_in), message(message_in) {}
} ValidityStatus;

typedef struct HandScore {
    int fu;
    int fan;
} HandScore;

class WinningHand {
  public:
    WinningHand(const ClassicHand &classic_hand, bool riichi = false,
                bool ippatsu = false, bool ron = false, int total_doras = 0);
    WinningHand(Tile seven_pairs_hand[7], bool riichi = false,
                bool ippatsu = false, bool ron = false, int total_doras = 0);
    WinningHand(Tile duo_orphans_hand, bool riichi = false,
                bool ippatsu = false, bool ron = false, int total_doras = 0);

    HandType type() const;
    HandTiles hand() const;
    bool isRiichi() const;
    bool isIppatsu() const;
    bool isRon() const;
    int totalDoras() const;
    QString toString() const;

    /* Scoring methods */
    ValidityStatus checkValid() const;
    HandScore computeScore() const;

  private:
    HandType type_;
    HandTiles hand_;
    bool riichi_;
    bool ippatsu_;
    bool ron_;
    int total_doras_;
};