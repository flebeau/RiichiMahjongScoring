#include "scoremodel.hpp"
#include <QBrush>
#include <iostream>

ScoreModel::ScoreModel(QObject *parent, N_Players _n_players,
                       int beginning_score, QString name_player_1,
                       QString name_player_2, QString name_player_3,
                       QString name_player_4)
    : QAbstractTableModel(parent), n_players_(_n_players) {
    scores_.push_back(
        std::vector<int>(static_cast<int>(n_players_), beginning_score));
    player_names_ = std::vector<QString>(4, QString());
    player_names_[0] = name_player_1;
    player_names_[1] = name_player_2;
    player_names_[2] = name_player_3;
    player_names_[3] = name_player_4;
}

int ScoreModel::rowCount(const QModelIndex & /* parent */) const {
    return scores_.size();
}

int ScoreModel::columnCount(const QModelIndex & /* parent */) const {
    return scores_[0].size() + 1;
}

QVariant ScoreModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        QString cell_content;
        // Add positive or negative value change from the turn before
        if (index.column() < static_cast<int>(scores_[0].size())) {
            cell_content +=
                QString("%1").arg(scores_[index.row()][index.column()]);
            if (index.row() > 0 &&
                scores_[index.row()][index.column()] <
                    scores_[index.row() - 1][index.column()]) {
                cell_content += QString(" (-%2)").arg(
                    scores_[index.row() - 1][index.column()] -
                    scores_[index.row()][index.column()]);
            } else if (index.row() > 0 &&
                       scores_[index.row()][index.column()] >
                           scores_[index.row() - 1][index.column()]) {
                cell_content += QString(" (+%2)").arg(
                    scores_[index.row()][index.column()] -
                    scores_[index.row() - 1][index.column()]);
            }
        } else {
            int sum = 0;
            for (unsigned j = 0; j < scores_[0].size(); j++) {
                sum += scores_[index.row()][j];
            }
            cell_content = QString("%1").arg(sum);
        }
        return cell_content;
    } else if (role == Qt::BackgroundRole) {
        if (index.row() > 0 &&
            index.column() < static_cast<int>(scores_[0].size())) {
            if (scores_[index.row()][index.column()] <
                scores_[index.row() - 1][index.column()]) {
                return QBrush(Qt::red);
            } else if (scores_[index.row()][index.column()] >
                       scores_[index.row() - 1][index.column()]) {
                return QBrush(Qt::green);
            }
        }
    }

    return QVariant();
}

QVariant ScoreModel::headerData(int section, Qt::Orientation orientation,
                                int role) const {
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            if (section == static_cast<int>(scores_[0].size())) {
                return QString(tr("Total"));
            } else {
                return QString("%1").arg(player_names_[section]);
            }
        } else if (orientation == Qt::Vertical) {
            if (section == 0) {
                return QString(tr("Initial"));
            } else {
                return QString(tr("Turn %1")).arg(section);
            }
        }
    }

    return QVariant();
}

const ScoreModel::N_Players &ScoreModel::NPlayers() const { return n_players_; }
const std::vector<QString> &ScoreModel::PlayerNames() const {
    return player_names_;
}

void ScoreModel::addTurnResult(const TurnResult &turn_result) {
    turn_results_.push_back(turn_result);
    recomputeScores();
}

void ScoreModel::recomputeScores() {
    std::vector<int> initial_scores = std::vector<int>(scores_[0]);
    scores_.clear();
    scores_.push_back(initial_scores);

    // Compute each line of score depending on the result of each turn
    for (size_t i = 0; i < turn_results_.size(); i++) {
        std::vector<int> turn_score_change =
            turn_results_[i].computeScoreChange(static_cast<int>(n_players_));
        scores_.push_back(std::vector<int>(scores_[0].size(), 0));
        for (size_t j = 0; j < scores_[0].size(); j++) {
            scores_[scores_.size() - 1][j] =
                scores_[scores_.size() - 2][j] + turn_score_change[j];
        }
    }
    emit layoutChanged();
}