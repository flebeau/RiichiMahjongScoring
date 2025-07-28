#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QTabWidget>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <vector>

#include "scoremodel.hpp"
#include "winning_hand.hpp"

/**
 * @brief Implements the dialog for adding the result of a turn
 *
 */
class AddResultDialog : public QDialog {
    Q_OBJECT
  public:
    AddResultDialog(QWidget *parent, ScoreModel::N_Players _n_players,
                    const std::vector<QString> &_player_names);

    /* Information getters */
    /**
     * @brief Returns the number of the East player selected
     *
     * @return int between 0 and 3 (included)
     */
    int EastPlayer() const;
    /**
     * @brief Returns the number of the winner
     *
     * @return int between 0 and 3 (included)
     */
    int Winner() const;
    /**
     * @brief Identify whether the win is by ron or tsumo
     *
     * @return true if it's a ron victory
     * @return false if it's a tsumo victory
     */
    int RonVictory() const;
    /**
     * @brief Return the manual scores entered in manual score tab
     *
     * @return std::vector<int>
     */
    std::vector<int> ManualScores() const;
    /**
     * @brief Returns the number of the loser in case of a ron victory
     *
     * @return int between 0 and 3 (included)
     */
    int Loser() const;
    /**
     * @brief Identify whether Player 1 did Riichi or not
     *
     * @return true if Player 1 did Riichi
     * @return false otherwise
     */
    bool Player1DidRiichi() const;
    /**
     * @brief Identify whether Player 2 did Riichi or not
     *
     * @return true if Player 2 did Riichi
     * @return false otherwise
     */
    bool Player2DidRiichi() const;
    /**
     * @brief Identify whether Player 3 did Riichi or not
     *
     * @return true if Player 3 did Riichi
     * @return false otherwise
     */
    bool Player3DidRiichi() const;
    /**
     * @brief Identify whether Player 4 did Riichi or not
     *
     * @return true if Player 4 did Riichi
     * @return false otherwise
     */
    bool Player4DidRiichi() const;

    /**
     * @brief Returns the entered Fu score
     */
    unsigned FuScore() const;
    /**
     * @brief Returns the entered Fan score
     */
    unsigned FanScore() const;

    const WinningHand *winningHand() const;

  private slots:
    /**
     * @brief Refresh the list of potential losers depending on the selected
     * winner
     */
    void refreshLoserSelector();
    /**
     * @brief Show the How to Score message box
     */
    void showHelp();
    void showHandDialog();

  private:
    /**
     * @brief Create the East selector
     *
     * @return QGroupBox*
     */
    QGroupBox *createEastSelector();
    /**
     * @brief Create the winner selector
     *
     * @return QGroupBox*
     */
    QGroupBox *createWinnerSelector();
    /**
     * @brief Create the ron or tsumo selector
     *
     * @return QGroupBox*
     */
    QGroupBox *createRonOrTsumoSelector();
    /**
     * @brief Create the group for Riichi players input
     *
     * @return QGroupBox*
     */
    QGroupBox *createRiichiInput();
    /**
     * @brief Create the Fu and Fan score selector
     *
     * @return QGroupBox*
     */
    QGroupBox *createFuFanSelector();
    QCheckBox *winnerRiichiButton();
    bool WinnerDidRiichi();

    ScoreModel::N_Players n_players_;          /**< Number of players */
    const std::vector<QString> &player_names_; /**< Names of the players */

    /* Widgets used */
    QRadioButton *ron_button_;   /**< Button for ron victory */
    QRadioButton *tsumo_button_; /**< Button for tsumo victory */
    QLabel *label_loser_;        /**< Label for loser selector */
    QComboBox *east_selector_;   /**< East selector */
    QComboBox *winner_selector_; /**< Winner selector */
    QComboBox *loser_selector_;  /**< Loser selector in case of ron victory */
    QCheckBox *riichi_player_1_; /**< Riichi selector for Player 1 */
    QCheckBox *riichi_player_2_; /**< Riichi selector for Player 2 */
    QCheckBox *riichi_player_3_; /**< Riichi selector for Player 3 */
    QCheckBox *riichi_player_4_; /**< Riichi selector for Player 4 */
    QLabel *fu_label_;           /**< Fu label */
    QSpinBox *fu_selector_;      /**< Fu score selector */
    QLabel *fan_label_;          /**< Fan label */
    QSpinBox *fan_selector_;     /**< Fan score selector */
    QPushButton *hand_dialog_button_; /**< Button to open hand dialog */
    QLabel *label_manual_player_1_;   /**< Label for manual score */
    QSpinBox *score_manual_player_1_; /**< Manual score selector */
    QLabel *label_manual_player_2_;   /**< Label for manual score */
    QSpinBox *score_manual_player_2_; /**< Manual score selector */
    QLabel *label_manual_player_3_;   /**< Label for manual score */
    QSpinBox *score_manual_player_3_; /**< Manual score selector */
    QLabel *label_manual_player_4_;   /**< Label for manual score */
    QSpinBox *score_manual_player_4_; /**< Manual score selector */
    QTabWidget *tabs_;                /**< Tab widget */
    QWidget *ron_tsumo_tab_;          /**< Default tab */
    QWidget *manual_tab_;             /**< Manual tab */
    QPushButton *confirm_button_;     /**< Confirm button */
    QPushButton *cancel_button_;      /**< Cancel button */
    QPushButton *help_button_;        /**< Help button */

    WinningHand *hand_ = nullptr;
};