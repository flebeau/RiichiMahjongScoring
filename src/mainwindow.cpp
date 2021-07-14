#include <QtWidgets>

#include "mainwindow.hpp"

MainWindow::MainWindow() {
    createActions();
    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (confirmExit()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::newGame() {}

void MainWindow::about() {
    QMessageBox::about(this, tr("About MahjongScoring"),
                       tr("This application is created in order to ease the "
                          "computation of Riichi Mahjong scoring and keep "
                          "track of the score of the players during a game."));
}

void MainWindow::createActions() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *newGameAct = new QAction(tr("&New Game"), this);
    newGameAct->setShortcuts(QKeySequence::New);
    newGameAct->setStatusTip(tr("Create a new game"));
    connect(newGameAct, &QAction::triggered, this, &MainWindow::newGame);
    fileMenu->addAction(newGameAct);

    fileMenu->addSeparator();

    QAction *exitAct = new QAction(tr("&Quit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, &QAction::triggered, this, &MainWindow::close);
    fileMenu->addAction(exitAct);

    QAction *aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the MahjongSolving application About box"));
    connect(aboutAct, &QAction::triggered, this, &MainWindow::about);
    helpMenu->addAction(aboutAct);
}

bool MainWindow::confirmExit() {
    const QMessageBox::StandardButton ret = QMessageBox::warning(
        this, tr("Exit"), tr("Are you sure you want to exit?"),
        QMessageBox::Yes | QMessageBox::Cancel);

    switch (ret) {
    case QMessageBox::Yes:
        return true;
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return false;
}