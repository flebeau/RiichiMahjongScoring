#pragma once

#include <QDialog>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>

#include "scoremodel.hpp"
#include "turnresult.hpp"

/**
 * @brief Class representing the Show turn detail dialog
 */
class ShowDetailDialog : public QDialog {
  public:
    ShowDetailDialog(QWidget *parent, const TurnResult &turn_result,
                     const ScoreModel::N_Players &n_players,
                     const std::vector<QString> &player_names);

  private:
    QLabel *label_info_;

    QPushButton *ok_button_;
};