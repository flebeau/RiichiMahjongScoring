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
      riichi_player_1_(
          new QCheckBox(tr("%1 did Riichi").arg(player_names_[0]))),
      riichi_player_2_(
          new QCheckBox(tr("%1 did Riichi").arg(player_names_[1]))),
      riichi_player_3_(
          new QCheckBox(tr("%1 did Riichi").arg(player_names_[2]))),
      riichi_player_4_(
          new QCheckBox(tr("%1 did Riichi").arg(player_names_[3]))),
      fu_label_(new QLabel(tr("Fu score"))), fu_selector_(new QSpinBox),
      fan_label_(new QLabel(tr("Bonus fan score"))),
      fan_selector_(new QSpinBox),
      label_manual_player_1_(new QLabel(tr("%1 scored").arg(player_names_[0]))),
      score_manual_player_1_(new QSpinBox),
      label_manual_player_2_(new QLabel(tr("%1 scored").arg(player_names_[1]))),
      score_manual_player_2_(new QSpinBox),
      label_manual_player_3_(new QLabel(tr("%1 scored").arg(player_names_[2]))),
      score_manual_player_3_(new QSpinBox),
      score_manual_player_4_(new QSpinBox), tabs_(new QTabWidget),
      ron_tsumo_tab_(new QWidget), manual_tab_(new QWidget),
      confirm_button_(new QPushButton(tr("&Confirm"))),
      cancel_button_(new QPushButton(tr("C&ancel"))),
      help_button_(new QPushButton(tr("&How to Score"))) {
    /* Initialize widgets for default tab */
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

    /* Initialize widgets for manual tab */
    // Set the minimum and maximum for spinboxes
    score_manual_player_1_->setMinimum(-100000);
    score_manual_player_1_->setMaximum(100000);
    score_manual_player_1_->setValue(0);
    score_manual_player_1_->setSingleStep(100);
    score_manual_player_2_->setMinimum(-100000);
    score_manual_player_2_->setMaximum(100000);
    score_manual_player_2_->setValue(0);
    score_manual_player_2_->setSingleStep(100);
    score_manual_player_3_->setMinimum(-100000);
    score_manual_player_3_->setMaximum(100000);
    score_manual_player_3_->setValue(0);
    score_manual_player_3_->setSingleStep(100);
    score_manual_player_4_->setMinimum(-100000);
    score_manual_player_4_->setMaximum(100000);
    score_manual_player_4_->setValue(0);
    score_manual_player_4_->setSingleStep(100);

    /* Create the layout for ron/tsumo tab */
    QVBoxLayout *default_tab_layout = new QVBoxLayout;

    default_tab_layout->addWidget(createEastSelector());
    default_tab_layout->addWidget(createWinnerSelector());
    default_tab_layout->addWidget(createRonOrTsumoSelector());
    default_tab_layout->addWidget(createRiichiInput());
    default_tab_layout->addWidget(createFuFanSelector());

    ron_tsumo_tab_->setLayout(default_tab_layout);

    /* Create the layout for the manual score tab */
    QGridLayout *manual_tab_layout = new QGridLayout;

    manual_tab_layout->addWidget(label_manual_player_1_, 0, 0);
    manual_tab_layout->addWidget(score_manual_player_1_, 0, 1);
    manual_tab_layout->addWidget(label_manual_player_2_, 1, 0);
    manual_tab_layout->addWidget(score_manual_player_2_, 1, 1);
    manual_tab_layout->addWidget(label_manual_player_3_, 2, 0);
    manual_tab_layout->addWidget(score_manual_player_3_, 2, 1);
    if (n_players_ == ScoreModel::N_Players::FOUR_PLAYERS) {
        label_manual_player_4_ =
            new QLabel(tr("%1 scored").arg(player_names_[3]));
        manual_tab_layout->addWidget(label_manual_player_4_, 3, 0);
        manual_tab_layout->addWidget(score_manual_player_4_, 3, 1);
    }
    manual_tab_layout->addWidget(new QWidget, 3, 0, 2, 2);

    manual_tab_->setLayout(manual_tab_layout);

    /* Add tabs to the tab widget */
    tabs_->addTab(ron_tsumo_tab_, tr("Ron or tsumo"));
    tabs_->addTab(manual_tab_, tr("Manual score"));

    /* Create the confirm / cancel layout */
    QVBoxLayout *confirm_cancel_layout = new QVBoxLayout;
    confirm_cancel_layout->addWidget(confirm_button_);
    confirm_cancel_layout->addWidget(cancel_button_);
    confirm_cancel_layout->addWidget(help_button_);

    /* Create the main layout */
    QGridLayout *main_layout = new QGridLayout;
    main_layout->addWidget(tabs_, 0, 0, 2, 1);
    main_layout->addLayout(confirm_cancel_layout, 0, 1);

    setLayout(main_layout);

    /* Set default button */
    confirm_button_->setDefault(true);

    setWindowTitle(tr("Adding a round result"));
}

int AddResultDialog::EastPlayer() const {
    const QString &east = east_selector_->currentText();
    if (east == player_names_[0]) {
        return 0;
    } else if (east == player_names_[1]) {
        return 1;
    } else if (east == player_names_[2]) {
        return 2;
    } else {
        return 3;
    }
}

int AddResultDialog::Winner() const {
    const QString &winner = winner_selector_->currentText();
    if (winner == player_names_[0]) {
        return 0;
    } else if (winner == player_names_[1]) {
        return 1;
    } else if (winner == player_names_[2]) {
        return 2;
    } else {
        return 3;
    }
}

int AddResultDialog::RonVictory() const {
    if (tabs_->currentIndex() == 0) {
        return static_cast<int>(ron_button_->isChecked());
    } else {
        return 2;
    }
}

std::vector<int> AddResultDialog::ManualScores() const {
    std::vector<int> scores = std::vector<int>(static_cast<int>(n_players_), 0);
    scores[0] = score_manual_player_1_->value();
    scores[1] = score_manual_player_2_->value();
    scores[2] = score_manual_player_3_->value();
    if (n_players_ == ScoreModel::N_Players::FOUR_PLAYERS) {
        scores[3] = score_manual_player_4_->value();
    }
    return scores;
}

int AddResultDialog::Loser() const {
    const QString &loser = loser_selector_->currentText();
    if (loser == player_names_[0]) {
        return 0;
    } else if (loser == player_names_[1]) {
        return 1;
    } else if (loser == player_names_[2]) {
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