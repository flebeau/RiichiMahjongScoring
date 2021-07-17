#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QVBoxLayout>
#include <iostream>

#include "addresultdialog.hpp"
#include "mainwidget.hpp"

MainWidget::MainWidget(QWidget *parent, ScoreModel *_score_model)
    : QWidget(parent), n_players_(_score_model->NPlayers()),
      player_names_(_score_model->PlayerNames()), score_model_(_score_model),
      score_view_(new QTableView),
      add_result_button_(new QPushButton(tr("&Add turn result"))),
      delete_result_button_(new QPushButton(tr("&Delete turn result"))),
      result_detail_button_(new QPushButton(tr("&Turn result detail"))) {
    // Link view with score model
    score_view_->setModel(_score_model);

    // Set score view stretch parameters
    score_view_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Define the layout of the main widget
    QGridLayout *main_layout = new QGridLayout;
    main_layout->addWidget(score_view_, 0, 0, 4, 1);
    main_layout->addWidget(add_result_button_, 0, 1);
    main_layout->addWidget(delete_result_button_, 1, 1);
    main_layout->addWidget(result_detail_button_, 2, 1);

    // Set the layout of the main widget
    setLayout(main_layout);

    // Connect the click of add result button to the spawn of the add result
    // dialog
    connect(add_result_button_, &QPushButton::clicked, this,
            &MainWidget::addResult);

    // Connect a click on table view to tableSelectionChanged slot
    connect(score_view_->selectionModel(),
            &QItemSelectionModel::selectionChanged, this,
            &MainWidget::tableClicked);

    // Initially, detail and delete buttons are disabled
    delete_result_button_->setDisabled(true);
    result_detail_button_->setDisabled(true);

    // Connect a click on delete button to the action of deleting a turn
    connect(delete_result_button_, &QPushButton::clicked, this,
            &MainWidget::deleteResult);
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

void MainWidget::tableClicked() {
    // Update visibility of detail and delete turn buttons
    if (!(score_view_->selectionModel()->isSelected(
            score_view_->selectionModel()->currentIndex())) ||
        score_view_->selectionModel()->currentIndex().row() == 0) {
        // In this case, detail and delete buttons should be disabled
        delete_result_button_->setDisabled(true);
        result_detail_button_->setDisabled(true);
    } else if (score_view_->selectionModel()->currentIndex().row() > 0) {
        // In this case, detail and delete buttons should be enabled
        delete_result_button_->setDisabled(false);
        result_detail_button_->setDisabled(false);
    }
}

void MainWidget::deleteResult() {
    // Ask confirmation
    int row_selected = score_view_->selectionModel()->currentIndex().row();
    const QMessageBox::StandardButton ret = QMessageBox::warning(
        this, tr("Delete turn result"),
        tr("Are you sure you want to delete result of turn %1?")
            .arg(row_selected),
        QMessageBox::Yes | QMessageBox::Cancel);

    if (ret != QMessageBox::Yes) {
        return;
    }

    // Delete the turn in the model
    score_model_->deleteTurnResult(row_selected - 1);
}

void MainWidget::showTurnDetail() {
    int row_selected = score_view_->selectionModel()->currentIndex().row();
}