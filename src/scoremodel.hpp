#pragma once
#include <QAbstractTableModel>
#include <vector>

#include "turnresult.hpp"

/**
 * @brief Abstract table model representing a game's scoresheet
 *
 */
class ScoreModel : public QAbstractTableModel {
    Q_OBJECT
  public:
    enum class N_Players { THREE_PLAYERS = 3, FOUR_PLAYERS = 4 };

    ScoreModel(QObject *parent, N_Players _n_players = N_Players::THREE_PLAYERS,
               int beginning_score = 30000,
               QString name_player_1 = QString("Player 1"),
               QString name_player_2 = QString("Player 2"),
               QString name_player_3 = QString("Player 3"),
               QString name_player_4 = QString("Player 4"));

    /* Inherited from QAbstractTableModel */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const;

    /* Getters */
    const N_Players &NPlayers() const;
    const std::vector<QString> &PlayerNames() const;

    /* Turn history modifiers */
    void addTurnResult(const TurnResult &turn_result);
    void deleteTurnResult(int turn_index);

    /* Empty the model and change number of players and player names */
    void reset(N_Players _n_players, int beginning_score,
               const std::vector<QString> &_player_names);

  private:
    /**
     * @brief Recompute the scores depending on the turn history
     */
    void recomputeScores();

    N_Players n_players_;                  /**< Number of players */
    std::vector<QString> player_names_;    /**< Names of the players */
    std::vector<TurnResult> turn_results_; /**< Turn results history */
    /** Saved scores: scores_[i][j] corresponds to the score of player j on
     * the i-th turn*/
    std::vector<std::vector<int>> scores_;
};