#include <QtWidgets>
#include <iostream>

#include "handdialog.hpp"
#include "howtoscoredialog.hpp"
#include "mainwindow.hpp"
#include "newgamedialog.hpp"

MainWindow::MainWindow()
    : score_model_(this), main_widget_(new MainWidget(this, &score_model_)) {
    /* Create Menus */
    createActions();
    setUnifiedTitleAndToolBarOnMac(true);

    /* Set principal widget */
    setCentralWidget(main_widget_);

    // Resize window
    resize(700, 700);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (confirmExit()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::newGame() {
    NewGameDialog new_game_dialog(this);

    if (new_game_dialog.exec() == QDialog::Accepted) {
        // Load new game information
        score_model_.reset(new_game_dialog.nPlayers(),
                           new_game_dialog.beginningScore(),
                           new_game_dialog.playersName());
        // Reset save file
        current_save_file_ = "";
    }
}

void MainWindow::about() {
    QMessageBox::about(
        this, tr("About RiichiMahjongScoring"),
        tr("This application is created in order to ease the "
           "computation of Riichi Mahjong scoring and keep "
           "track of the score of the players during a game.\n\n The scoring "
           "computation is based on Scott D. Miller's book The Ultimate "
           "Guide "
           "to the Japanese Game Taking the World by Storm."));
}

void MainWindow::howToScore() {
    HowToScoreDialog how_to_score_dialog;

    how_to_score_dialog.exec();
}

void MainWindow::save() {
    // If no current save file, call saveAs()
    if (current_save_file_.isEmpty()) {
        saveAs();
    }

    // Try to open the file in write mode
    QFile file(current_save_file_);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("Unable to open file"),
                                 file.errorString());
        current_save_file_ = "";
        return;
    }

    // Serialize the score model to the file
    QTextStream out(&file);
    score_model_.writeToTextStream(out);
}

void MainWindow::saveAs() {
    // Ask for file dialog
    current_save_file_ = QFileDialog::getSaveFileName(
        this, tr("Save scoresheet"), "",
        tr("Mahjong Scoresheet (*.mss);;All Files (*)"));

    // Save scoresheet to file
    if (!current_save_file_.isEmpty()) {
        save();
    }
}

void MainWindow::load() {
    // Ask for file dialog
    QString load_file = QFileDialog::getOpenFileName(
        this, tr("Load scoresheet"), "",
        tr("Mahjong Scoresheet (*.mss);;All Files(*)"));

    // Load scoresheet from file
    if (!load_file.isEmpty()) {
        QFile file(load_file);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }
        QTextStream in(&file);
        if (!score_model_.loadFromTextStream(in)) {
            QMessageBox::warning(this, tr("Wrong format"),
                                 tr("Error while parsing file"));
            return;
        }
        current_save_file_ = load_file;
    }
}

void MainWindow::testHandSelector() {
    HandDialog dialog(this);
    dialog.exec();
}

void MainWindow::createActions() {
    /* Create Menus */
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    /* New Game action */
    const QIcon newIcon = QIcon::fromTheme("document-new");
    QAction *newGameAct = new QAction(newIcon, tr("&New Game"), this);
    newGameAct->setShortcuts(QKeySequence::New);
    newGameAct->setStatusTip(tr("Create a new game"));
    connect(newGameAct, &QAction::triggered, this, &MainWindow::newGame);
    fileMenu->addAction(newGameAct);

    /* Load game from file action */
    const QIcon loadIcon = QIcon::fromTheme("document-open");
    QAction *loadAct = new QAction(loadIcon, tr("&Load from file"), this);
    loadAct->setShortcuts(QKeySequence::Open);
    loadAct->setStatusTip(tr("Load a scoresheet from a file"));
    connect(loadAct, &QAction::triggered, this, &MainWindow::load);
    fileMenu->addAction(loadAct);

    /* Save action */
    const QIcon saveIcon = QIcon::fromTheme("document-save");
    QAction *saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save current scoresheet to file"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);

    /* Save As Action */
    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    QAction *saveAsAct = new QAction(saveAsIcon, tr("S&ave As"), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save current scoresheet to file"));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
    fileMenu->addAction(saveAsAct);

    fileMenu->addSeparator();

    /* Exit action */
    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = new QAction(exitIcon, tr("&Quit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, &QAction::triggered, this, &MainWindow::close);
    fileMenu->addAction(exitAct);

    /* About action */
    const QIcon aboutIcon = QIcon::fromTheme("help-about");
    QAction *aboutAct = new QAction(aboutIcon, tr("&About"), this);
    aboutAct->setStatusTip(
        tr("Show the RiichiMahjongSolving application About box"));
    connect(aboutAct, &QAction::triggered, this, &MainWindow::about);
    helpMenu->addAction(aboutAct);

    /* How to score action */
    const QIcon howToIcon = QIcon::fromTheme("help-faq");
    QAction *howToAct = new QAction(howToIcon, tr("&How to Score"), this);
    howToAct->setShortcuts(QKeySequence::HelpContents);
    howToAct->setStatusTip(tr("Show the How to Score help page"));
    connect(howToAct, &QAction::triggered, this, &MainWindow::howToScore);
    helpMenu->addAction(howToAct);
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