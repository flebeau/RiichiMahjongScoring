#pragma once
#include <QDialog>
#include <QPushButton>
#include <QTableView>

#include "scoremodel.hpp"

/**
 * @brief Class defining the main widget of the application
 *
 * It consists of a scoresheet and buttons to add a line of results
 */
class MainWidget : public QWidget {
    Q_OBJECT
  public:
    MainWidget(QWidget *parent, ScoreModel *_score_model);

  private slots:
    /**
     * @brief Slot for when adding a turn result
     */
    void addResult();
    /**
     * @brief Slot for when the table selection changed
     */
    void tableClicked();
    /**
     * @brief Slot for when deleting a turn result
     */
    void deleteResult();

  private:
    const ScoreModel::N_Players &n_players_;   /**< Number of players */
    const std::vector<QString> &player_names_; /**< Player names */

    ScoreModel *score_model_;           /**< The scoresheet model */
    QTableView *score_view_;            /**< The scoresheet view */
    QPushButton *add_result_button_;    /**< The "Add result" button */
    QPushButton *delete_result_button_; /**< The "Delete result" button */
    QPushButton *result_detail_button_; /**< The "Result detail" button */
};