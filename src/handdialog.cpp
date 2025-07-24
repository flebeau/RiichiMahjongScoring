#include "handdialog.hpp"
#include "tile.hpp"
#include <functional>
#include <qnamespace.h>

TileSelector::TileSelector(QWidget *parent, bool chii_selector)
    : QWidget(parent), chii_(chii_selector), suit_(new QComboBox),
      value_(new QComboBox) {
    suit_->addItem("Bamboo", BAMBOO);
    suit_->addItem("Characters", CHARACTER);
    suit_->addItem("Dots", DOT);
    if (!chii_selector) {
        suit_->addItem("Honor", HONOR);
    }

    connect(suit_, &QComboBox::currentTextChanged, this,
            &TileSelector::suitChanged);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(suit_);
    layout->addWidget(value_);

    setLayout(layout);
    suitChanged();
}

Tile TileSelector::value() const {
    return Tile(suit_->itemData(suit_->currentIndex()).toChar().unicode(),
                value_->itemData(value_->currentIndex()).toInt());
}

void TileSelector::setChii(bool chii) {
    if (chii_ == chii) {
        return;
    }
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
    suitChanged();
}

void TileSelector::suitChanged() {
    value_->clear();
    if (suit_->itemData(suit_->currentIndex()).toChar() == HONOR) {
        value_->addItem(" E ", static_cast<int>(HonorValue::EAST));
        value_->addItem(" S ", static_cast<int>(HonorValue::SOUTH));
        value_->addItem(" W ", static_cast<int>(HonorValue::WEST));
        value_->addItem(" N ", static_cast<int>(HonorValue::NORTH));
        value_->addItem("Wh D", static_cast<int>(HonorValue::WHITE));
        value_->addItem("Gr D", static_cast<int>(HonorValue::GREEN));
        value_->addItem("Re D", static_cast<int>(HonorValue::RED));
    } else {
        for (int i = 1; i <= (chii_ ? 7 : 9); ++i) {
            value_->addItem("  " + QString::number(i) + "  ", i);
        }
    }
    value_->updateGeometry();
}

ClassicGroup::ClassicGroup(QWidget *parent)
    : QGroupBox(parent), type_(new QComboBox),
      first_tile_(new TileSelector(nullptr, true)),
      melded_(new QCheckBox("Melded")) {
    type_->addItem("Chi");
    type_->addItem("Pon");
    type_->addItem("Kan");
    connect(type_, &QComboBox::currentTextChanged, this,
            &ClassicGroup::onTypeChanged);
    connect(this, &ClassicGroup::typeChanged, first_tile_,
            &TileSelector::setChii);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setAlignment(Qt::AlignCenter);
    layout->addWidget(type_);
    layout->addWidget(first_tile_);
    layout->addWidget(melded_);
    setLayout(layout);
    setTitle("Group");
}

bool ClassicGroup::isChi() const { return type_->currentText() == "Chi"; }
bool ClassicGroup::isPon() const { return type_->currentText() == "Pon"; }
bool ClassicGroup::isKan() const { return type_->currentText() == "Kan"; }
bool ClassicGroup::isMelded() const { return melded_->isChecked(); }

Tile ClassicGroup::firstTile() const { return first_tile_->value(); }

void ClassicGroup::onTypeChanged() { emit typeChanged(isChi()); }

DuoGroup::DuoGroup(QWidget *parent)
    : QGroupBox(parent), tile_selector_(new TileSelector(this, false)),
      melded_(new QCheckBox("Melded")) {
    melded_->setChecked(false);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(tile_selector_);
    layout->addWidget(melded_);
    layout->setAlignment(Qt::AlignCenter);
    setLayout(layout);
    setTitle("Duo");
}

Tile DuoGroup::tile() const { return tile_selector_->value(); }
bool DuoGroup::isMelded() const { return melded_->isChecked(); }

HandDialog::HandDialog(QWidget *parent)
    : QDialog(parent), tabs_(new QTabWidget), classic_tab_(new QWidget),
      seven_pairs_tab_(new QWidget), thirteen_orphans_tab_(new QWidget),
      first_group(new ClassicGroup), second_group(new ClassicGroup),
      third_group(new ClassicGroup), fourth_group(new ClassicGroup),
      duo_group(new DuoGroup) {
    /* Create the classic tab */
    QVBoxLayout *classic_layout = new QVBoxLayout;

    classic_layout->addWidget(first_group);
    classic_layout->addWidget(second_group);
    classic_layout->addWidget(third_group);
    classic_layout->addWidget(fourth_group);
    first_group->setTitle("First group");
    second_group->setTitle("Second group");
    third_group->setTitle("Third group");
    fourth_group->setTitle("Fourth group");
    classic_layout->addWidget(duo_group);
    classic_tab_->setLayout(classic_layout);
    tabs_->addTab(classic_tab_, "Classic");

    /* Create the seven pairs tab */
    QGridLayout *seven_pairs_layout = new QGridLayout;
    for (int i = 0; i < 6; i++) {
        seven_pairs_groups_[i] = new DuoGroup;
        seven_pairs_layout->addWidget(seven_pairs_groups_[i], i / 2,
                                      2 * (i % 2), 1, 2);
    }
    seven_pairs_groups_[6] = new DuoGroup;
    seven_pairs_layout->addWidget(seven_pairs_groups_[6], 3, 1, 1, 2);
    seven_pairs_tab_->setLayout(seven_pairs_layout);
    tabs_->addTab(seven_pairs_tab_, "Seven pairs");

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(tabs_);
    setLayout(layout);
    setWindowTitle("Hand");
}