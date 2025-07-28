#pragma once

#include "tile.hpp"
#include <QVector>
#include <qvector.h>

enum class HandType { CLASSIC, PAIRS, ORPHANS };

enum class ClassicGroupType { CHII, PON, KAN };

typedef struct ClassicGroup {
    ClassicGroupType type;
    Tile tile;
    bool melded = false;
    bool ron_meld = false;
    ClassicGroup(const ClassicGroupType &type_in = ClassicGroupType::CHII,
                 const Tile &tile_in = Tile(), bool melded_in = false,
                 bool ron_meld_in = false)
        : type(type_in), tile(tile_in), melded(melded_in),
          ron_meld(ron_meld_in) {}
    QString toString() const;
    bool isSimple() const;
} ClassicGroup;

typedef struct ClassicHand {
    ClassicGroup groups[4];
    Tile duo_tile;
    QString toString() const;
    ClassicHand(const ClassicGroup &first_group,
                const ClassicGroup &second_group,
                const ClassicGroup &third_group,
                const ClassicGroup &fourth_group, const Tile &duo_tile)
        : groups{first_group, second_group, third_group, fourth_group},
          duo_tile(duo_tile) {}
} ClassicHand;

union HandTiles {
    ClassicHand classic_hand;
    Tile seven_pairs_hand[7];
    Tile duo_orphans_hand;

    HandTiles() {}
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

typedef struct ValueDetail {
    int value;
    QString detail;
    ValueDetail(int value_in = 0, const QString &detail_in = "")
        : value(value_in), detail(detail_in) {}
} ValueDetail;

static const int MANGAN = 5;
static const int YAKUMAN = 13;

class HandScore {
  public:
    HandScore();

    int totalFu() const;
    int totalFan() const;
    const QVector<ValueDetail> &fuDetails() const;
    const QVector<ValueDetail> &yakus() const;

    void addFu(int fu, const QString &detail);
    void addYaku(int fan, const QString &detail);
    void addBetterYaku(int fan, const QString &detail);
    void addYakuman(const QString &detail, bool doubled = false);

    QString toString() const;

  private:
    int fu_;
    QVector<ValueDetail> fu_details_;
    int fan_;
    QVector<ValueDetail> yakus_;
};

class WinningHand {
  public:
    WinningHand(){};
    WinningHand(const ClassicHand &classic_hand, const Tile &dominant_wind,
                const Tile &player_wind, bool riichi = false,
                bool ippatsu = false, bool ron = false, int total_doras = 0);
    WinningHand(Tile seven_pairs_hand[7], const Tile &dominant_wind,
                const Tile &player_wind, bool riichi = false,
                bool ippatsu = false, bool ron = false, int total_doras = 0);
    WinningHand(Tile duo_orphans_hand, const Tile &dominant_wind,
                const Tile &player_wind, bool riichi = false,
                bool ippatsu = false, bool ron = false, int total_doras = 0);
    WinningHand(const QString &description);

    HandType type() const;
    HandTiles hand() const;
    bool isRiichi() const;
    bool isIppatsu() const;
    bool isRon() const;
    bool isTsumo() const;
    bool isClosed() const;
    bool isOpen() const;
    int totalDoras() const;
    QString toString() const;
    const Tile &dominantWind() const;
    const Tile &playerWind() const;

    /* Scoring methods */
    ValidityStatus checkValid() const;
    HandScore computeScore() const;

    /* String utils */
    static QString windTileToString(const Tile &tile);
    static Tile stringToWindTile(const QString &str);

  private:
    HandType type_;
    HandTiles hand_;
    bool riichi_;
    bool ippatsu_;
    bool ron_;
    int total_doras_;
    Tile dominant_wind_;
    Tile player_wind_;
};