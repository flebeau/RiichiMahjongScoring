#include "newgamedialog.hpp"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

NewGameDialog::NewGameDialog(QWidget *parent)
    : QDialog(parent), n_players_(ScoreModel::N_Players::THREE_PLAYERS),
      three_players_button_(new QRadioButton(tr("Three Players"))),
      four_players_button_(new QRadioButton(tr("Four Players"))),
      player_1_name_edit_(new QLineEdit), player_2_name_edit_(new QLineEdit),
      player_3_name_edit_(new QLineEdit), player_4_name_edit_(new QLineEdit),
      n_players_selector_(new QGroupBox(tr("Number of players"))),
      player_names_selector_(new QGroupBox(tr("Player names"))),
      label_beginning_score_(new QLabel(tr("Beginning score: "))),
      beginning_score_selector_(new QSpinBox),
      confirm_button_(new QPushButton(tr("&Confirm"))),
      cancel_button_(new QPushButton(tr("C&ancel"))) {
    /* Init number of players group box */
    three_players_button_->setChecked(true);

    connect(three_players_button_, &QRadioButton::clicked, this,
            &NewGameDialog::nPlayerChanged);
    connect(four_players_button_, &QRadioButton::clicked, this,
            &NewGameDialog::nPlayerChanged);

    QHBoxLayout *n_players_layout = new QHBoxLayout;
    n_players_layout->addWidget(three_players_button_);
    n_players_layout->addWidget(four_players_button_);
    n_players_selector_->setLayout(n_players_layout);

    /* Init player names selector */
    player_1_name_edit_->setText("Player 1");
    player_2_name_edit_->setText("Player 2");
    player_3_name_edit_->setText("Player 3");
    player_4_name_edit_->setText("Player 4");
    player_4_name_edit_->hide();

    QVBoxLayout *player_names_layout = new QVBoxLayout;
    player_names_layout->addWidget(player_1_name_edit_);
    player_names_layout->addWidget(player_2_name_edit_);
    player_names_layout->addWidget(player_3_name_edit_);
    player_names_layout->addWidget(player_4_name_edit_);
    player_names_selector_->setLayout(player_names_layout);

    /* Init beginning score selector */
    beginning_score_selector_->setMinimum(0);
    beginning_score_selector_->setMaximum(1000000000);
    beginning_score_selector_->setValue(30000);

    QHBoxLayout *beginning_score_layout = new QHBoxLayout;
    beginning_score_layout->addWidget(label_beginning_score_);
    beginning_score_layout->addWidget(beginning_score_selector_);

    /* Init confirm and cancel buttons */
    connect(confirm_button_, &QPushButton::clicked, this,
            &NewGameDialog::accept);
    connect(cancel_button_, &QPushButton::clicked, this,
            &NewGameDialog::reject);

    QVBoxLayout *confirm_cancel_layout = new QVBoxLayout;
    confirm_cancel_layout->addWidget(confirm_button_);
    confirm_cancel_layout->addWidget(cancel_button_);

    /* Set main layout of dialog */
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(n_players_selector_, 0, 0);
    layout->addWidget(player_names_selector_, 1, 0);
    layout->addLayout(beginning_score_layout, 2, 0);
    layout->addLayout(confirm_cancel_layout, 0, 1);

    /* Set default button and dialog title */
    confirm_button_->setDefault(true);

    setWindowTitle(tr("New game"));

    setLayout(layout);
}

const ScoreModel::N_Players &NewGameDialog::nPlayers() const {
    return n_players_;
}

std::vector<QString> NewGameDialog::playersName() const {
    std::vector<QString> res;
    res.push_back(player_1_name_edit_->text());
    res.push_back(player_2_name_edit_->text());
    res.push_back(player_3_name_edit_->text());
    if (n_players_ == ScoreModel::N_Players::FOUR_PLAYERS) {
        res.push_back(player_4_name_edit_->text());
    }
    return res;
}

int NewGameDialog::beginningScore() const {
    return beginning_score_selector_->value();
}

void NewGameDialog::nPlayerChanged() {
    if (three_players_button_->isChecked()) {
        n_players_ = ScoreModel::N_Players::THREE_PLAYERS;
        player_4_name_edit_->hide();
    } else if (four_players_button_->isChecked()) {
        n_players_ = ScoreModel::N_Players::FOUR_PLAYERS;
        player_4_name_edit_->show();
    }
}