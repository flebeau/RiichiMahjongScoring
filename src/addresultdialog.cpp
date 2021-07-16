#include <QGridLayout>
#include <QHBoxLayout>

#include "addresultdialog.hpp"
#include "howtoscoredialog.hpp"

AddResultDialog::AddResultDialog(QWidget *parent,
                                 ScoreModel::N_Players _n_players,
                                 const std::vector<QString> &_player_names)
    : QDialog(parent), n_players_(_n_players), player_names_(_player_names),
      ron_button_(new QRadioButton(tr("Ron"))),
      tsumo_button_(new QRadioButton(tr("Tsumo"))), label_loser_(new QLabel),
      east_selector_(new QComboBox), winner_selector_(new QComboBox),
      loser_selector_(new QComboBox),
      riichi_player_1_(new QCheckBox(tr("Player 1 did Riichi"))),
      riichi_player_2_(new QCheckBox(tr("Player 2 did Riichi"))),
      riichi_player_3_(new QCheckBox(tr("Player 3 did Riichi"))),
      riichi_player_4_(new QCheckBox(tr("Player 4 did Riichi"))),
      fu_label_(new QLabel(tr("Fu score"))), fu_selector_(new QSpinBox),
      fan_label_(new QLabel(tr("Bonus fan score"))),
      fan_selector_(new QSpinBox),
      confirm_button_(new QPushButton(tr("&Confirm"))),
      cancel_button_(new QPushButton(tr("C&ancel"))),
      help_button_(new QPushButton(tr("&How to Score"))) {
    /* Initialize widgets */
    // Load names of players for winner selector and east selector
    winner_selector_->addItem(player_names_[0]);
    winner_selector_->addItem(player_names_[1]);
    winner_selector_->addItem(player_names_[2]);
    if (n_players_ == ScoreModel::N_Players::FOUR_PLAYERS) {
        winner_selector_->addItem(player_names_[3]);
    }
    east_selector_->addItem(player_names_[0]);
    east_selector_->addItem(player_names_[1]);
    east_selector_->addItem(player_names_[2]);
    if (n_players_ == ScoreModel::N_Players::FOUR_PLAYERS) {
        east_selector_->addItem(player_names_[3]);
    }

    // Check tsumo victory button
    tsumo_button_->setChecked(true);

    // Connect tsumo, ron buttons and winner selector  with refresh of loser
    // selector
    connect(tsumo_button_, &QRadioButton::clicked, this,
            &AddResultDialog::refreshLoserSelector);
    connect(ron_button_, &QRadioButton::clicked, this,
            &AddResultDialog::refreshLoserSelector);
    connect(winner_selector_, &QComboBox::currentTextChanged, this,
            &AddResultDialog::refreshLoserSelector);

    // Connect confirm and cancel buttons
    connect(confirm_button_, &QPushButton::clicked, this,
            &AddResultDialog::accept);
    connect(cancel_button_, &QPushButton::clicked, this,
            &AddResultDialog::reject);
    connect(help_button_, &QPushButton::clicked, this,
            &AddResultDialog::showHelp);

    // Refresh the loser selector and set the loser label
    refreshLoserSelector();
    label_loser_->setText(tr("Loser: "));

    // Set the minimum and maximum for Fu and Fan scores
    fu_selector_->setMinimum(20);
    fu_selector_->setMaximum(90);
    fan_selector_->setMinimum(1);

    /* Create the confirm / cancel layout */
    QVBoxLayout *confirm_cancel_layout = new QVBoxLayout;
    confirm_cancel_layout->addWidget(confirm_button_);
    confirm_cancel_layout->addWidget(cancel_button_);
    confirm_cancel_layout->addWidget(help_button_);

    /* Create the main layout */
    QGridLayout *layout = new QGridLayout;

    layout->addWidget(createEastSelector(), 0, 0);
    layout->addWidget(createWinnerSelector(), 1, 0);
    layout->addWidget(createRonOrTsumoSelector(), 2, 0);
    layout->addWidget(createRiichiInput(), 3, 0);
    layout->addWidget(createFuFanSelector(), 4, 0);
    layout->addLayout(confirm_cancel_layout, 0, 1);

    setLayout(layout);

    setWindowTitle(tr("Adding a round result"));
}

int AddResultDialog::EastPlayer() const {
    const QString &east = east_selector_->currentText();
    if (east == "Player 1") {
        return 0;
    } else if (east == "Player 2") {
        return 1;
    } else if (east == "Player 3") {
        return 2;
    } else {
        return 3;
    }
}

int AddResultDialog::Winner() const {
    const QString &winner = winner_selector_->currentText();
    if (winner == "Player 1") {
        return 0;
    } else if (winner == "Player 2") {
        return 1;
    } else if (winner == "Player 3") {
        return 2;
    } else {
        return 3;
    }
}

bool AddResultDialog::RonVictory() const { return ron_button_->isChecked(); }

int AddResultDialog::Loser() const {
    const QString &loser = loser_selector_->currentText();
    if (loser == "Player 1") {
        return 0;
    } else if (loser == "Player 2") {
        return 1;
    } else if (loser == "Player 3") {
        return 2;
    } else {
        return 3;
    }
}

bool AddResultDialog::Player1DidRiichi() const {
    return riichi_player_1_->isChecked();
}
bool AddResultDialog::Player2DidRiichi() const {
    return riichi_player_2_->isChecked();
}
bool AddResultDialog::Player3DidRiichi() const {
    return riichi_player_3_->isChecked();
}
bool AddResultDialog::Player4DidRiichi() const {
    return riichi_player_4_->isChecked();
}

unsigned AddResultDialog::FuScore() const { return fu_selector_->value(); }
unsigned AddResultDialog::FanScore() const { return fan_selector_->value(); }

void AddResultDialog::refreshLoserSelector() {
    // If ron button is not checked, loser selector is disabled
    if (!ron_button_->isChecked()) {
        loser_selector_->setDisabled(true);
        return;
    } else {
        loser_selector_->setEnabled(true);
    }
    // Empty items
    loser_selector_->clear();
    // Load possible losers
    if (player_names_[0] != winner_selector_->currentText()) {
        loser_selector_->addItem(player_names_[0]);
    }
    if (player_names_[1] != winner_selector_->currentText()) {
        loser_selector_->addItem(player_names_[1]);
    }
    if (player_names_[2] != winner_selector_->currentText()) {
        loser_selector_->addItem(player_names_[2]);
    }
    if (n_players_ == ScoreModel::N_Players::FOUR_PLAYERS &&
        player_names_[3] != winner_selector_->currentText()) {
        loser_selector_->addItem(player_names_[3]);
    }
}

void AddResultDialog::showHelp() {
    HowToScoreDialog how_to_score_dialog;

    how_to_score_dialog.exec();
}

QGroupBox *AddResultDialog::createEastSelector() {
    QGroupBox *group_box = new QGroupBox(tr("East player"));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(east_selector_);

    group_box->setLayout(layout);
    return group_box;
}

QGroupBox *AddResultDialog::createWinnerSelector() {
    QGroupBox *group_box = new QGroupBox(tr("Winner"));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(winner_selector_);

    group_box->setLayout(layout);
    return group_box;
}

QGroupBox *AddResultDialog::createRonOrTsumoSelector() {
    QGroupBox *group_box = new QGroupBox(tr("Victory by"));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(tsumo_button_);
    layout->addWidget(ron_button_);
    layout->addWidget(label_loser_);
    layout->addWidget(loser_selector_);

    group_box->setLayout(layout);
    return group_box;
}

QGroupBox *AddResultDialog::createRiichiInput() {
    QGroupBox *group_box = new QGroupBox(tr("Riichi information"));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(riichi_player_1_);
    layout->addWidget(riichi_player_2_);
    layout->addWidget(riichi_player_3_);
    if (n_players_ == ScoreModel::N_Players::FOUR_PLAYERS) {
        layout->addWidget(riichi_player_4_);
    }

    group_box->setLayout(layout);
    return group_box;
}

QGroupBox *AddResultDialog::createFuFanSelector() {
    QGroupBox *group_box = new QGroupBox(tr("Fu / Fan score"));

    QGridLayout *layout = new QGridLayout;

    layout->addWidget(fu_label_, 0, 0);
    layout->addWidget(fu_selector_, 0, 1);
    layout->addWidget(fan_label_, 1, 0);
    layout->addWidget(fan_selector_, 1, 1);

    group_box->setLayout(layout);
    return group_box;
}