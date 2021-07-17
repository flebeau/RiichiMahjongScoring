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

void ScoreModel::deleteTurnResult(int turn_index) {
    turn_results_.erase(turn_results_.begin() + turn_index);
    recomputeScores();
}

void ScoreModel::reset(N_Players _n_players, int beginning_score,
                       const std::vector<QString> &_player_names) {
    // Empty turns and scores
    turn_results_.clear();
    scores_.clear();

    // Change number of players
    n_players_ = _n_players;

    // Initialize scores
    scores_.push_back(
        std::vector<int>(static_cast<int>(n_players_), beginning_score));

    // Change player names
    player_names_[0] = _player_names[0];
    player_names_[1] = _player_names[1];
    player_names_[2] = _player_names[2];
    if (n_players_ == N_Players::FOUR_PLAYERS) {
        player_names_[3] = _player_names[3];
    }

    emit layoutChanged();
}

void ScoreModel::writeToTextStream(QTextStream &out) const {
    out << static_cast<int>(n_players_) << "\n"
        << player_names_[0] << "\n"
        << player_names_[1] << "\n"
        << player_names_[2] << "\n";
    if (n_players_ == N_Players::FOUR_PLAYERS) {
        out << player_names_[3] << "\n";
    }
    out << scores_[0][0] << "\n";
    for (unsigned i = 0; i < turn_results_.size(); ++i) {
        turn_results_[i].writeToTextStream(out);
        out << "\n";
    }
}

bool ScoreModel::loadFromTextStream(QTextStream &in) {
    /* Read basic information */
    int read_n_players = 0;
    in >> read_n_players;
    if (read_n_players < 3 || read_n_players > 4) {
        return false;
    }
    QString line;
    std::vector<QString> player_names =
        std::vector<QString>(read_n_players, "");
    // End reading first line
    in.readLineInto(&line);
    /* Read player names  */
    if (!in.readLineInto(&line)) {
        return false;
    }
    player_names[0] = line;
    if (!in.readLineInto(&line)) {
        return false;
    }
    player_names[1] = line;
    if (!in.readLineInto(&line)) {
        return false;
    }
    player_names[2] = line;
    if (read_n_players > 3) {
        if (!in.readLineInto(&line)) {
            return false;
        }
        player_names[3] = line;
    }
    int beginning_score;
    in >> beginning_score;
    if (beginning_score < 0) {
        return false;
    }
    // Read end of line with beginning score
    in.readLineInto(&line);

    /* Reset the scoresheet */
    reset(static_cast<N_Players>(read_n_players), beginning_score,
          player_names);

    /* Read the turn results */
    while (in.readLineInto(&line)) {
        turn_results_.push_back(TurnResult(&line));
    }

    // Recompute scores
    recomputeScores();

    return true;
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