#include "tile.hpp"

Tile::Tile(char suit, int value) : suit_(suit), value_(value) {}
QString Tile::toString() const { return QString::number(value_) + suit_; }

char Tile::suit() const { return suit_; }
int Tile::value() const { return value_; }
bool Tile::isHonor() const { return suit_ == HONOR; }
bool Tile::isTerminal() const {
    return !isHonor() && (value_ == 1 || value_ == 9);
}