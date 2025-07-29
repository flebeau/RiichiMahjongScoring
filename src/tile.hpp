#pragma once

#include <QString>

const char BAMBOO = 's';
const char CHARACTER = 'm';
const char DOT = 'p';
const char HONOR = 'z';

enum class HonorValue {
    EAST = 1,
    SOUTH = 2,
    WEST = 3,
    NORTH = 4,
    WHITE = 5,
    GREEN = 6,
    RED = 7
};

class Tile {
  public:
    Tile(char suit = BAMBOO, int value = 1);
    Tile(const QString &descr);
    QString toString() const;
    char suit() const;
    int value() const;
    bool isHonor() const;
    bool isDragon() const;
    bool isWind() const;
    bool isTerminal() const;
    bool isOrphan() const;

    bool operator==(const Tile &other) const;

  private:
    char suit_;
    int value_;
};