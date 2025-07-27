#include "handdialog.hpp"
#include "winning_hand.hpp"
#include <QtWidgets>
#include <functional>
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
    connect(first_tile_, &TileSelector::Changed, this,
            &ClassicGroupSelector::onChange);
    connect(melded_, &QCheckBox::stateChanged, this,
            &ClassicGroupSelector::onChange);
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

ClassicGroup ClassicGroupSelector::value() const {
    if (isChi()) {
        return ClassicGroup(ClassicGroupType::CHII, firstTile(), isMelded());
    }
    if (isPon()) {
        return ClassicGroup(ClassicGroupType::PON, firstTile(), isMelded());
    }
    return ClassicGroup(ClassicGroupType::KAN, firstTile(), isMelded());
}

void ClassicGroupSelector::onTypeChanged() {
    emit typeChanged(isChi());
    emit Changed();
}

void ClassicGroupSelector::onChange() { emit Changed(); }

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

void DuoGroupSelector::onChange() { emit Changed(); }

Tile DuoGroupSelector::tile() const { return tile_selector_->value(); }

HandDialog::HandDialog(QWidget *parent)
    : QDialog(parent), tabs_(new QTabWidget), classic_tab_(new QWidget),
      seven_pairs_tab_(new QWidget), thirteen_orphans_tab_(new QWidget),
      first_group_(new ClassicGroupSelector),
      second_group_(new ClassicGroupSelector),
      third_group_(new ClassicGroupSelector),
      fourth_group_(new ClassicGroupSelector), duo_group_(new DuoGroupSelector),
      ron_button_(new QRadioButton(tr("Ron"))),
      tsumo_button_(new QRadioButton(tr("Tsumo"))), doras_(new QSpinBox),
      riichi_button_(new QCheckBox(tr("Riichi"))),
      ippatsu_button_(new QCheckBox(tr("Ippatsu"))),
      score_text_(new QLabel("")) {
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
    tsumo_button_->setChecked(true);
    win_by_layout->addWidget(tsumo_button_);
    win_by_layout->addWidget(ron_button_);
    riichi_button_->setChecked(false);
    win_by_layout->addWidget(riichi_button_);
    ippatsu_button_->setEnabled(false);
    win_by_layout->addWidget(ippatsu_button_);
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

    connect(riichi_button_, &QRadioButton::toggled, this,
            &HandDialog::onRiichiChange);

    score_text_->setWordWrap(true);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(tabs_, 0, 0, 2, 1);
    layout->addLayout(victory_infos_layout, 0, 1, 1, 1);
    layout->addWidget(score_text_, 1, 1, 1, 1, Qt::AlignTop);
    layout->setRowStretch(0, 0);
    layout->setRowStretch(1, 1);
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
    // connect(doras_, &QSpinBox::valueChanged, this, &HandDialog::onChange);
    connect(tsumo_button_, &QRadioButton::toggled, this, &HandDialog::onChange);
    connect(riichi_button_, &QCheckBox::toggled, this, &HandDialog::onChange);
    connect(ippatsu_button_, &QCheckBox::toggled, this, &HandDialog::onChange);
    // TODO Add update with dora change
    updateScoreText();
}

void HandDialog::updateScoreText() {
    HandScore score = hand_represented_.computeScore();
    QString text = "<hr><p><b>Fu:</b> " + QString::number(score.totalFu()) +
                   (score.fuDetails().size() > 0 ? " = 20" : "");
    for (const auto &fu_detail : score.fuDetails()) {
        text += " + " + QString::number(fu_detail.value);
    }

    text += "<br><b>Fan:</b> " + QString::number(score.totalFan()) +
            "</p>\n<b>Yakus:</b>\n<ul>\n";
    // Add yaku names
    for (const auto &yaku : score.yakus()) {
        text += "  <li>" + yaku.detail + " (+" + QString::number(yaku.value) +
                ")" + "</li>\n";
    }
    text += "</ul>";
    score_text_->setText(text);
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
            riichi_button_->isChecked(), ippatsu_button_->isChecked(),
            ron_button_->isChecked(), doras_->value());

    } else if (tabs_->currentIndex() == 1) {
        Tile duo_tiles[7];
        for (int i = 0; i < 7; i++) {
            duo_tiles[i] = seven_pairs_groups_[i]->tile();
        }
        hand_represented_ =
            WinningHand(duo_tiles, riichi_button_->isChecked(),
                        ippatsu_button_->isChecked(), ron_button_->isChecked(),
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