#pragma once

#include <QDialog>
#include <QLabel>
#include <QPushButton>

/**
 * @brief Class representing the "How to Score" help dialog
 */
class HowToScoreDialog : public QDialog {
  public:
    HowToScoreDialog();

  private:
    QLabel *left_text_;      /**< Computing Fu and Fan text */
    QLabel *right_text_;     /**< Computing Yaku text */
    QPushButton *ok_button_; /**< Ok button */
};