#pragma once
#include <QMainWindow>

#include "mainwidget.hpp"
#include "scoremodel.hpp"

/**
 * @brief Main window of the application
 *
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow();

  protected:
    /**
     * @brief Override close event in order to ask for confirmation
     *
     * @param event
     */
    void closeEvent(QCloseEvent *event) override;

  private slots:
    /**
     * @brief New Game slot
     */
    void newGame();
    /**
     * @brief About message box slot
     */
    void about();
    /**
     * @brief How to Score message dialog slot
     */
    void howToScore();
    /**
     * @brief Save to file slot
     */
    void save();
    /**
     * @brief Save As slot
     */
    void saveAs();

  private:
    /**
     * @brief Create actions for the menu bar
     *
     */
    void createActions();
    /**
     * @brief Ask for confirmation on exit
     *
     * @return true if exit confirmed
     * @return false if exit cancelled
     */
    bool confirmExit();

    ScoreModel score_model_; /**< Scoresheet */
    MainWidget *main_widget_;
    QString current_save_file_;
};