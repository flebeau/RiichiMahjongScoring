#pragma once

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
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

  protected:
    void resizeEvent(QResizeEvent *event) override;

  private:
    QLabel *label_info_;

    QPushButton *ok_button_;
    QGraphicsScene *hand_scene_ = nullptr;
    QGraphicsView *hand_view_ = nullptr;
};