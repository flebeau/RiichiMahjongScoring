#pragma once
#include <QDialog>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>

#include "scoremodel.hpp"

/**
 * @brief Class describing the dialog for a new game
 */
class NewGameDialog : public QDialog {
    Q_OBJECT
  public:
    NewGameDialog(QWidget *parent);

    /**
     * @brief Returns the number of players
     */
    const ScoreModel::N_Players &nPlayers() const;
    /**
     * @brief Returns the name of the players as a vector of QString
     */
    std::vector<QString> playersName() const;
    /**
     * @brief Returns the beginning score entered by the user
     */
    int beginningScore() const;

  private slots:
    /**
     * @brief Slot called when the number of players selected changed
     *
     */
    void nPlayerChanged();

  private:
    ScoreModel::N_Players n_players_; /**< Number of players */

    /* Widgets */
    QRadioButton *three_players_button_;
    QRadioButton *four_players_button_;

    QLineEdit *player_1_name_edit_;
    QLineEdit *player_2_name_edit_;
    QLineEdit *player_3_name_edit_;
    QLineEdit *player_4_name_edit_;

    QGroupBox *n_players_selector_;
    QGroupBox *player_names_selector_;

    QLabel *label_beginning_score_;
    QSpinBox *beginning_score_selector_;

    QPushButton *confirm_button_;
    QPushButton *cancel_button_;
};