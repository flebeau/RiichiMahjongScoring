#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QVBoxLayout>
#include <iostream>

#include "addresultdialog.hpp"
#include "mainwidget.hpp"

MainWidget::MainWidget(QWidget *parent, ScoreModel *_score_model)
    : QWidget(parent), n_players_(_score_model->NPlayers()),
      player_names_(_score_model->PlayerNames()), score_model_(_score_model),
      score_view_(new QTableView),
      add_result_button_(new QPushButton(tr("&Add result"))),
      result_detail_button_(new QPushButton(tr("Result &Detail"))) {
    // Link view with score model
    score_view_->setModel(_score_model);

    // Set score view stretch parameters
    score_view_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Define the layout of the main widget
    QGridLayout *main_layout = new QGridLayout;
    main_layout->addWidget(score_view_, 0, 0, 3, 1);
    main_layout->addWidget(add_result_button_, 0, 1);
    main_layout->addWidget(result_detail_button_, 1, 1);

    // Set the layout of the main widget
    setLayout(main_layout);

    // Connect the click of add result button to the spawn of the add result
    // dialog
    connect(add_result_button_, &QPushButton::clicked, this,
            &MainWidget::addResult);
}

void MainWidget::addResult() {
    AddResultDialog add_result_dialog(this, n_players_, player_names_);

    // Popup the add result dialog
    if (add_result_dialog.exec() == QDialog::Accepted) {
        score_model_->addTurnResult(TurnResult(
            add_result_dialog.EastPlayer(), add_result_dialog.Winner(),
            add_result_dialog.RonVictory(), add_result_dialog.Loser(),
            add_result_dialog.Player1DidRiichi(),
            add_result_dialog.Player2DidRiichi(),
            add_result_dialog.Player3DidRiichi(),
            add_result_dialog.Player4DidRiichi(), add_result_dialog.FuScore(),
            add_result_dialog.FanScore()));
        score_view_->resizeColumnsToContents();
    }
}