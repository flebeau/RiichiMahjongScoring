#include "tile.hpp"

Tile::Tile(char suit, int value) : suit_(suit), value_(value) {}
Tile::Tile(const QString &descr) {
    suit_ = descr[1].unicode();
    value_ = descr[0].digitValue();
}
QString Tile::toString() const { return QString::number(value_) + suit_; }

char Tile::suit() const { return suit_; }
int Tile::value() const { return value_; }
bool Tile::isHonor() const { return suit_ == HONOR; }
bool Tile::isDragon() const { return isHonor() && (value_ >= 5); }
bool Tile::isWind() const { return isHonor() && (value_ <= 4); }
bool Tile::isTerminal() const {
    return !isHonor() && (value_ == 1 || value_ == 9);
}
bool Tile::isOrphan() const { return isHonor() || isTerminal(); }

bool Tile::operator==(const Tile &other) const {
    return suit_ == other.suit() && value_ == other.value();
}