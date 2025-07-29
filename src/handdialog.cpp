#include "handdialog.hpp"
#include "tile.hpp"
#include "winning_hand.hpp"
#include <QtWidgets>
#include <functional>
#include <qboxlayout.h>
#include <qcheckbox.h>
#include <qgridlayout.h>
#include <qnamespace.h>
#include <qtabwidget.h>

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
            &TileSelector::onSuitChange);
    connect(value_, &QComboBox::currentTextChanged, this,
            &TileSelector::onValueChange);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(suit_);
    layout->addWidget(value_);

    setLayout(layout);
    onSuitChange();
}

Tile TileSelector::value() const {
    return Tile(suit_->itemData(suit_->currentIndex()).toChar().unicode(),
                value_->itemData(value_->currentIndex()).toInt());
}

void TileSelector::setValue(const Tile &tile) {
    suit_->setCurrentIndex(suit_->findData(tile.suit()));
    value_->setCurrentIndex(value_->findData(tile.value()));
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
    onSuitChange();
}

void TileSelector::onSuitChange() {
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
    emit Changed();
}

void TileSelector::onValueChange() { emit Changed(); }

ClassicGroupSelector::ClassicGroupSelector(QWidget *parent, bool ron_possible)
    : QGroupBox(parent), type_(new QComboBox),
      first_tile_(new TileSelector(nullptr, true)),
      melded_(new QCheckBox(tr("Melded"))), ron_prevented_(false) {
    type_->addItem("Chi");
    type_->addItem("Pon");
    type_->addItem("Kan");
    connect(type_, &QComboBox::currentTextChanged, this,
            &ClassicGroupSelector::onTypeChanged);
    connect(this, &ClassicGroupSelector::typeChanged, first_tile_,
            &TileSelector::setChii);
    connect(first_tile_, &TileSelector::Changed, this,
            &ClassicGroupSelector::onChange);
    connect(melded_, &QCheckBox::stateChanged, this,
            &ClassicGroupSelector::onMeldedChange);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setAlignment(Qt::AlignCenter);
    layout->addWidget(type_);
    layout->addWidget(first_tile_);
    layout->addWidget(melded_);
    if (ron_possible) {
        ron_ = new QCheckBox(tr("Ron"));
        connect(ron_, &QCheckBox::toggled, this,
                &ClassicGroupSelector::onRonChange);
        layout->addWidget(ron_);
        ron_->setEnabled(false);
    } else {
        ron_ = nullptr;
    }
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
bool ClassicGroupSelector::isRonMelded() const {
    if (ron_ == nullptr) {
        return false;
    }
    return melded_->isChecked() && ron_->isChecked();
}

Tile ClassicGroupSelector::firstTile() const { return first_tile_->value(); }

ClassicGroup ClassicGroupSelector::value() const {
    if (isChi()) {
        return ClassicGroup(ClassicGroupType::CHII, firstTile(), isMelded(),
                            isRonMelded());
    }
    if (isPon()) {
        return ClassicGroup(ClassicGroupType::PON, firstTile(), isMelded(),
                            isRonMelded());
    }
    return ClassicGroup(ClassicGroupType::KAN, firstTile(), isMelded(),
                        isRonMelded());
}

void ClassicGroupSelector::setValue(const ClassicGroup &group) {
    if (group.type == ClassicGroupType::CHII) {
        type_->setCurrentText("Chi");
    } else if (group.type == ClassicGroupType::PON) {
        type_->setCurrentText("Pon");
    } else {
        type_->setCurrentText("Kan");
    }
    first_tile_->setValue(group.tile);
    melded_->setChecked(group.melded);
    if (ron_ != nullptr) {
        ron_->setChecked(group.ron_meld);
    }
}

void ClassicGroupSelector::preventRon(bool prevent) {
    ron_prevented_ = prevent;
    if (ron_ != nullptr) {
        if (ron_prevented_) {
            ron_->setChecked(false);
            ron_->setEnabled(false);
        } else if (melded_->isChecked()) {
            ron_->setEnabled(true);
        }
    }
}

void ClassicGroupSelector::onTypeChanged() {
    emit typeChanged(isChi());
    emit Changed();
}

void ClassicGroupSelector::onChange() { emit Changed(); }

void ClassicGroupSelector::onMeldedChange() {
    if (ron_ != nullptr) {
        ron_->setChecked(false);
        ron_->setEnabled(melded_->isChecked() && !ron_prevented_);
    }
    emit Changed();
}

void ClassicGroupSelector::onRonChange() {
    if (ron_ != nullptr) {
        emit ronChanged(ron_->isChecked());
    }
    emit Changed();
}

DuoGroupSelector::DuoGroupSelector(QWidget *parent)
    : QGroupBox(parent), tile_selector_(new TileSelector(this, false)) {
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(tile_selector_);
    layout->setAlignment(Qt::AlignCenter);
    setLayout(layout);
    setTitle("Duo");
    connect(tile_selector_, &TileSelector::Changed, this,
            &DuoGroupSelector::onChange);
}

void DuoGroupSelector::setTile(const Tile &tile) {
    tile_selector_->setValue(tile);
}

void DuoGroupSelector::onChange() { emit Changed(); }

Tile DuoGroupSelector::tile() const { return tile_selector_->value(); }

HandDialog::HandDialog(QWidget *parent, const WinningHand *hand, bool ron,
                       bool east_player, bool riichi)
    : QDialog(parent), ron_(ron), tabs_(new QTabWidget),
      classic_tab_(new QWidget), seven_pairs_tab_(new QWidget),
      thirteen_orphans_tab_(new QWidget),
      first_group_(new ClassicGroupSelector(this, ron)),
      second_group_(new ClassicGroupSelector(this, ron)),
      third_group_(new ClassicGroupSelector(this, ron)),
      fourth_group_(new ClassicGroupSelector(this, ron)),
      duo_group_(new DuoGroupSelector), doras_(new QSpinBox),
      riichi_button_(new QCheckBox(tr("Riichi"))),
      ippatsu_button_(new QCheckBox(tr("Ippatsu"))),
      dominant_wind_label_(new QLabel(tr("Dominant"))),
      player_wind_label_(new QLabel(tr("Player"))),
      dominant_wind_selector_(new QComboBox),
      player_wind_selector_(new QComboBox), score_text_(new QLabel("")),
      confirm_button_(new QPushButton(tr("&Confirm"))),
      cancel_button_(new QPushButton(tr("C&ancel"))) {
    /* Create the classic tab */
    QVBoxLayout *classic_layout = new QVBoxLayout;

    classic_layout->addWidget(first_group_);
    classic_layout->addWidget(second_group_);
    classic_layout->addWidget(third_group_);
    classic_layout->addWidget(fourth_group_);
    first_group_->setTitle("First group");
    second_group_->setTitle("Second group");
    third_group_->setTitle("Third group");
    fourth_group_->setTitle("Fourth group");
    classic_layout->addWidget(duo_group_);
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

    QGroupBox *win_by = new QGroupBox(tr("Win by"));
    QHBoxLayout *win_by_layout = new QHBoxLayout;
    riichi_button_->setChecked(false);
    win_by_layout->addWidget(riichi_button_, 0);
    ippatsu_button_->setEnabled(false);
    win_by_layout->addWidget(ippatsu_button_, 1);
    win_by_layout->setSpacing(30);
    win_by->setLayout(win_by_layout);

    victory_infos_layout->addWidget(win_by, 0, 0, 1, 2);
    doras_->setMinimum(0);
    doras_->setMaximum(99);
    doras_->setValue(0);
    QGroupBox *doras = new QGroupBox(tr("Doras (total)"));
    QHBoxLayout *doras_layout = new QHBoxLayout;
    doras_layout->addWidget(doras_);
    doras->setLayout(doras_layout);
    victory_infos_layout->addWidget(doras, 1, 0, 1, 1);

    dominant_wind_selector_->addItem(" E ", static_cast<int>(HonorValue::EAST));
    dominant_wind_selector_->addItem(" S ",
                                     static_cast<int>(HonorValue::SOUTH));
    dominant_wind_selector_->addItem(" W ", static_cast<int>(HonorValue::WEST));
    dominant_wind_selector_->addItem(" N ",
                                     static_cast<int>(HonorValue::NORTH));
    if (east_player) {
        player_wind_selector_->addItem(" E ",
                                       static_cast<int>(HonorValue::EAST));
    }
    player_wind_selector_->addItem(" S ", static_cast<int>(HonorValue::SOUTH));
    player_wind_selector_->addItem(" W ", static_cast<int>(HonorValue::WEST));
    player_wind_selector_->addItem(" N ", static_cast<int>(HonorValue::NORTH));
    if (east_player) {
        player_wind_selector_->setCurrentIndex(0);
        player_wind_selector_->setEnabled(false);
    }
    QGroupBox *winds_group = new QGroupBox(tr("Winds"));
    QGridLayout *winds = new QGridLayout;
    winds->addWidget(dominant_wind_label_, 0, 0, 1, 1, Qt::AlignRight);
    winds->addWidget(dominant_wind_selector_, 0, 1, 1, 1);
    winds->addWidget(player_wind_label_, 1, 0, 1, 1, Qt::AlignRight);
    winds->addWidget(player_wind_selector_, 1, 1, 1, 1);
    winds->setHorizontalSpacing(20);
    winds_group->setLayout(winds);
    victory_infos_layout->addWidget(winds_group, 1, 1, 1, 1);

    connect(riichi_button_, &QRadioButton::toggled, this,
            &HandDialog::onRiichiChange);

    score_text_->setWordWrap(true);

    QHBoxLayout *buttons_layout = new QHBoxLayout;
    buttons_layout->addWidget(confirm_button_);
    buttons_layout->addWidget(cancel_button_);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(tabs_, 0, 0, 3, 1);
    layout->addLayout(victory_infos_layout, 0, 1, 1, 1);
    layout->addWidget(score_text_, 1, 1, 1, 1, Qt::AlignTop);
    layout->addLayout(buttons_layout, 2, 1, 1, 1, Qt::AlignBottom);
    layout->setRowStretch(0, 0);
    layout->setRowStretch(1, 1);
    layout->setRowStretch(2, 0);
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 1);
    setLayout(layout);
    setWindowTitle(tr("Winning Hand"));

    connect(tabs_, &QTabWidget::currentChanged, this, &HandDialog::onChange);
    for (const auto &widget :
         {first_group_, second_group_, third_group_, fourth_group_})
        connect(widget, &ClassicGroupSelector::Changed, this,
                &HandDialog::onChange);
    connect(duo_group_, &DuoGroupSelector::Changed, this,
            &HandDialog::onChange);
    for (const auto &duo : seven_pairs_groups_)
        connect(duo, &DuoGroupSelector::Changed, this, &HandDialog::onChange);
    // connect(doras_, &QSpinBox::valueChanged, this,
    // &HandDialog::onChange);
    connect(riichi_button_, &QCheckBox::toggled, this, &HandDialog::onChange);
    connect(ippatsu_button_, &QCheckBox::toggled, this, &HandDialog::onChange);
    connect(dominant_wind_selector_, &QComboBox::currentTextChanged, this,
            &HandDialog::onChange);
    connect(player_wind_selector_, &QComboBox::currentTextChanged, this,
            &HandDialog::onChange);
    // TODO Add update with dora change

    /* Only one ron meld */
    connect(first_group_, &ClassicGroupSelector::ronChanged, second_group_,
            &ClassicGroupSelector::preventRon);
    connect(first_group_, &ClassicGroupSelector::ronChanged, third_group_,
            &ClassicGroupSelector::preventRon);
    connect(first_group_, &ClassicGroupSelector::ronChanged, fourth_group_,
            &ClassicGroupSelector::preventRon);
    connect(second_group_, &ClassicGroupSelector::ronChanged, first_group_,
            &ClassicGroupSelector::preventRon);
    connect(second_group_, &ClassicGroupSelector::ronChanged, third_group_,
            &ClassicGroupSelector::preventRon);
    connect(second_group_, &ClassicGroupSelector::ronChanged, fourth_group_,
            &ClassicGroupSelector::preventRon);
    connect(third_group_, &ClassicGroupSelector::ronChanged, first_group_,
            &ClassicGroupSelector::preventRon);
    connect(third_group_, &ClassicGroupSelector::ronChanged, second_group_,
            &ClassicGroupSelector::preventRon);
    connect(third_group_, &ClassicGroupSelector::ronChanged, fourth_group_,
            &ClassicGroupSelector::preventRon);
    connect(fourth_group_, &ClassicGroupSelector::ronChanged, first_group_,
            &ClassicGroupSelector::preventRon);
    connect(fourth_group_, &ClassicGroupSelector::ronChanged, second_group_,
            &ClassicGroupSelector::preventRon);
    connect(fourth_group_, &ClassicGroupSelector::ronChanged, third_group_,
            &ClassicGroupSelector::preventRon);

    connect(confirm_button_, &QPushButton::clicked, this, &HandDialog::accept);
    connect(cancel_button_, &QPushButton::clicked, this, &HandDialog::reject);

    if (riichi) {
        riichi_button_->setChecked(true);
        riichi_button_->setEnabled(false);
    }

    if (hand != nullptr) {
        if (hand->type() == HandType::CLASSIC) {
            tabs_->setCurrentIndex(0);
            const ClassicHand &classic_hand = hand->hand().classic_hand;
            first_group_->setValue(classic_hand.groups[0]);
            second_group_->setValue(classic_hand.groups[1]);
            third_group_->setValue(classic_hand.groups[2]);
            fourth_group_->setValue(classic_hand.groups[3]);
            duo_group_->setTile(classic_hand.duo_tile);
        } else if (hand->type() == HandType::PAIRS) {
            tabs_->setCurrentIndex(1);
            const auto &groups = hand->hand().seven_pairs_hand;
            for (int i = 0; i < 7; i++) {
                seven_pairs_groups_[i]->setTile(groups[i]);
            }
        } else if (hand->type() == HandType::ORPHANS) {
            // TODO
        }
        if (hand->isRiichi()) {
            riichi_button_->setChecked(true);
        }
        if (hand->isIppatsu()) {
            ippatsu_button_->setChecked(true);
        }
        doras_->setValue(hand->totalDoras());
        player_wind_selector_->setCurrentIndex(
            player_wind_selector_->findData(hand->playerWind().value()));
        dominant_wind_selector_->setCurrentIndex(
            dominant_wind_selector_->findData(hand->dominantWind().value()));
    }
    onChange();
}

const WinningHand &HandDialog::hand() const { return hand_represented_; }

void HandDialog::updateScoreText() {
    HandScore score = hand_represented_.computeScore();
    score_text_->setText(score.toString());
}

void HandDialog::onChange() {
    if (tabs_->currentIndex() == 0) {
        ClassicGroup first_group = first_group_->value();
        ClassicGroup second_group = second_group_->value();
        ClassicGroup third_group = third_group_->value();
        ClassicGroup fourth_group = fourth_group_->value();
        Tile duo_tile = duo_group_->tile();
        hand_represented_ = WinningHand(
            ClassicHand(first_group, second_group, third_group, fourth_group,
                        duo_tile),
            Tile(HONOR, dominant_wind_selector_
                            ->itemData(dominant_wind_selector_->currentIndex())
                            .toInt()),
            Tile(HONOR, player_wind_selector_
                            ->itemData(player_wind_selector_->currentIndex())
                            .toInt()),
            riichi_button_->isChecked(), ippatsu_button_->isChecked(), ron_,
            doras_->value());

    } else if (tabs_->currentIndex() == 1) {
        Tile duo_tiles[7];
        for (int i = 0; i < 7; i++) {
            duo_tiles[i] = seven_pairs_groups_[i]->tile();
        }
        hand_represented_ = WinningHand(
            duo_tiles,
            Tile(HONOR, dominant_wind_selector_
                            ->itemData(dominant_wind_selector_->currentIndex())
                            .toInt()),
            Tile(HONOR, player_wind_selector_
                            ->itemData(player_wind_selector_->currentIndex())
                            .toInt()),
            riichi_button_->isChecked(), ippatsu_button_->isChecked(), ron_,
            doras_->value());
    }
    updateScoreText();
}

void HandDialog::onRiichiChange() {
    if (riichi_button_->isChecked()) {
        ippatsu_button_->setEnabled(true);
    } else {
        ippatsu_button_->setChecked(false);
        ippatsu_button_->setEnabled(false);
    }
}