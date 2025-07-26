#include "handdialog.hpp"
#include "tile.hpp"
#include <QtWidgets>
#include <functional>
#include <qgroupbox.h>

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

ClassicGroupSelector::ClassicGroupSelector(QWidget *parent)
    : QGroupBox(parent), type_(new QComboBox),
      first_tile_(new TileSelector(nullptr, true)),
      melded_(new QCheckBox("Melded")) {
    type_->addItem("Chi");
    type_->addItem("Pon");
    type_->addItem("Kan");
    connect(type_, &QComboBox::currentTextChanged, this,
            &ClassicGroupSelector::onTypeChanged);
    connect(this, &ClassicGroupSelector::typeChanged, first_tile_,
            &TileSelector::setChii);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setAlignment(Qt::AlignCenter);
    layout->addWidget(type_);
    layout->addWidget(first_tile_);
    layout->addWidget(melded_);
    setLayout(layout);
    setTitle("Group");
}

bool ClassicGroupSelector::isChi() const {
    return type_->currentText() == "Chi";
}
bool ClassicGroupSelector::isPon() const {
    return type_->currentText() == "Pon";
}
bool ClassicGroupSelector::isKan() const {
    return type_->currentText() == "Kan";
}
bool ClassicGroupSelector::isMelded() const { return melded_->isChecked(); }

Tile ClassicGroupSelector::firstTile() const { return first_tile_->value(); }

void ClassicGroupSelector::onTypeChanged() { emit typeChanged(isChi()); }

DuoGroupSelector::DuoGroupSelector(QWidget *parent)
    : QGroupBox(parent), tile_selector_(new TileSelector(this, false)) {
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(tile_selector_);
    layout->setAlignment(Qt::AlignCenter);
    setLayout(layout);
    setTitle("Duo");
}

Tile DuoGroupSelector::tile() const { return tile_selector_->value(); }

HandDialog::HandDialog(QWidget *parent)
    : QDialog(parent), tabs_(new QTabWidget), classic_tab_(new QWidget),
      seven_pairs_tab_(new QWidget), thirteen_orphans_tab_(new QWidget),
      first_group(new ClassicGroupSelector),
      second_group(new ClassicGroupSelector),
      third_group(new ClassicGroupSelector),
      fourth_group(new ClassicGroupSelector), duo_group(new DuoGroupSelector),
      ron_button_(new QRadioButton(tr("Ron"))),
      tsumo_button_(new QRadioButton(tr("Tsumo"))), doras_(new QSpinBox),
      riichi_button_(new QCheckBox(tr("Riichi"))),
      ippatsu_button_(new QCheckBox(tr("Ippatsu"))) {
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
        seven_pairs_groups_[i] = new DuoGroupSelector;
        seven_pairs_layout->addWidget(seven_pairs_groups_[i], i / 2,
                                      2 * (i % 2), 1, 2);
    }
    seven_pairs_groups_[6] = new DuoGroupSelector;
    seven_pairs_layout->addWidget(seven_pairs_groups_[6], 3, 1, 1, 2);
    seven_pairs_tab_->setLayout(seven_pairs_layout);
    tabs_->addTab(seven_pairs_tab_, "Seven pairs");

    /* Add other informations */
    QGridLayout *victory_infos_layout = new QGridLayout;

    QGroupBox *tsumo_ron = new QGroupBox(tr("Tsumo or Ron"));
    QHBoxLayout *tsumo_ron_layout = new QHBoxLayout;
    tsumo_ron_layout->addWidget(tsumo_button_);
    tsumo_ron_layout->addWidget(ron_button_);
    tsumo_ron->setLayout(tsumo_ron_layout);
    tsumo_button_->setChecked(true);

    victory_infos_layout->addWidget(tsumo_ron, 0, 0, 1, 2);
    doras_->setMinimum(0);
    doras_->setMaximum(99);
    doras_->setValue(0);
    QGroupBox *doras = new QGroupBox(tr("Doras (total)"));
    QHBoxLayout *doras_layout = new QHBoxLayout;
    doras_layout->addWidget(doras_);
    doras->setLayout(doras_layout);
    victory_infos_layout->addWidget(doras, 0, 2, 1, 1);

    QGroupBox *riichi = new QGroupBox(tr("Riichi"));
    QHBoxLayout *riichi_layout = new QHBoxLayout;
    riichi_layout->addWidget(riichi_button_);
    riichi_layout->addWidget(ippatsu_button_);
    riichi->setLayout(riichi_layout);
    victory_infos_layout->addWidget(riichi, 1, 0, 1, 1);
    connect(riichi_button_, &QRadioButton::toggled, ippatsu_button_,
            &QRadioButton::setEnabled);
    riichi_button_->setChecked(false);
    ippatsu_button_->setEnabled(false);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(tabs_);
    layout->addLayout(victory_infos_layout);
    setLayout(layout);
    setWindowTitle(tr("Winning Hand"));
}