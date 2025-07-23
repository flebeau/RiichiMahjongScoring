#include "handdialog.hpp"
#include "tile.hpp"
#include <QtWidgets>
#include <qboxlayout.h>

TileSelector::TileSelector(QWidget *parent, bool chii_selector)
    : QWidget(parent), chii_(chii_selector), suit_(new QComboBox),
      value_(new QComboBox) {
    suit_->addItem("Bamboo", BAMBOO);
    suit_->addItem("Characters", CHARACTER);
    suit_->addItem("Dots", DOT);
    if (!chii_selector) {
        suit_->addItem("Honor", HONOR);
    }

    connect(suit_, &QComboBox::currentIndexChanged, this,
            &TileSelector::suitChanged);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(suit_);
    layout->addWidget(value_);

    setLayout(layout);
}

Tile TileSelector::value() const {
    return Tile(suit_->itemData(suit_->currentIndex()).toChar().unicode(),
                value_->itemData(value_->currentIndex()).toInt());
}

void TileSelector::setChii(bool chii) {
    if (chii_ && !chii) {
        suit_->addItem("Honor", HONOR);
    } else if (!chii_ && chii) {
        int index = suit_->findData(HONOR);
        if (index == suit_->currentIndex()) {
            suit_->setCurrentIndex(0);
        }
        suit_->removeItem(index);
    }
    chii_ = chii;
}

void TileSelector::suitChanged() {
    value_->clear();
    if (suit_->itemData(suit_->currentIndex()).toChar() == HONOR) {
        value_->addItem("East", static_cast<int>(HonorValue::EAST));
        value_->addItem("South", static_cast<int>(HonorValue::SOUTH));
        value_->addItem("West", static_cast<int>(HonorValue::WEST));
        value_->addItem("North", static_cast<int>(HonorValue::NORTH));
        value_->addItem("White", static_cast<int>(HonorValue::WHITE));
        value_->addItem("Green", static_cast<int>(HonorValue::GREEN));
        value_->addItem("Red", static_cast<int>(HonorValue::RED));
    } else {
        for (int i = 1; i <= (chii_ ? 7 : 9); ++i) {
            value_->addItem(QString::number(i), i);
        }
    }
}

ClassicGroup::ClassicGroup(QWidget *parent)
    : QGroupBox(parent), chi_(new QRadioButton("Chi")),
      pon_(new QRadioButton("Pon")), kan_(new QRadioButton("Kan")),
      first_tile_(new TileSelector(this, true)),
      melded_(new QCheckBox("Melded")) {
    QVBoxLayout *radio_layout = new QVBoxLayout;
    radio_layout->addWidget(chi_);
    radio_layout->addWidget(pon_);
    radio_layout->addWidget(kan_);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addLayout(radio_layout);
    layout->addWidget(first_tile_);
    layout->addWidget(melded_);
    setLayout(layout);
}

bool ClassicGroup::isChi() const { return chi_->isChecked(); }
bool ClassicGroup::isPon() const { return pon_->isChecked(); }
bool ClassicGroup::isKan() const { return kan_->isChecked(); }
bool ClassicGroup::isMelded() const { return melded_->isChecked(); }

Tile ClassicGroup::firstTile() const { return first_tile_->value(); }

void ClassicGroup::typeChanged() { first_tile_->setChii(chii_->isChecked()); }

DuoGroup::DuoGroup(QWidget *parent)
    : QGroupBox(parent), tile_selector_(new TileSelector(this, false)),
      melded_(new QCheckBox("Melded")) {
    melded_->setChecked(false);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(tile_selector_);
    layout->addWidget(melded_);
    setLayout(layout);
}

Tile DuoGroup::tile() const { return tile_selector_->value(); }
bool DuoGroup::isMelded() const { return melded_->isChecked(); }

HandDialog::HandDialog(QWidget *parent)
    : QDialog(parent), hand_type(new QComboBox) {
    /* Create the widgets */
    hand_type->addItem("Classic");
    hand_type->addItem("Seven pairs");
    hand_type->addItem("Thirteen orphans");
}