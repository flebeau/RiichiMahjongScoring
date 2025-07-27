#pragma once

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <qcombobox.h>
#include <qlabel.h>
#include <qpushbutton.h>

#include "tile.hpp"
#include "winning_hand.hpp"

class TileSelector : public QWidget {
    Q_OBJECT
  public:
    TileSelector(QWidget *parent = nullptr, bool chii_selector = true);

    Tile value() const;
    void setValue(const Tile &tile);

  public slots:
    void setChii(bool chii);

  private slots:
    void onSuitChange();
    void onValueChange();

  signals:
    void Changed();

  private:
    bool chii_;
    QComboBox *suit_;
    QComboBox *value_;
};

class ClassicGroupSelector : public QGroupBox {
    Q_OBJECT
  public:
    ClassicGroupSelector(QWidget *parent = nullptr);

    bool isChi() const;
    bool isPon() const;
    bool isKan() const;
    bool isMelded() const;
    Tile firstTile() const;
    ClassicGroup value() const;
    void setValue(const ClassicGroup &group);

  signals:
    void typeChanged(bool chii);
    void Changed();

  private slots:
    void onTypeChanged();
    void onChange();

  private:
    QComboBox *type_;
    TileSelector *first_tile_;
    QCheckBox *melded_;
};

class DuoGroupSelector : public QGroupBox {
    Q_OBJECT
  public:
    DuoGroupSelector(QWidget *parent = nullptr);

    Tile tile() const;
    void setTile(const Tile &tile);
  private slots:
    void onChange();
  signals:
    void Changed();

  private:
    TileSelector *tile_selector_;
};

/**
 * @brief Implements the dialog for adding the winning hand of a turn
 *
 */
class HandDialog : public QDialog {
    Q_OBJECT
  public:
    HandDialog(QWidget *parent = nullptr, const WinningHand *hand = nullptr);
    const WinningHand &hand() const;

  private slots:
    void onChange();
    void onRiichiChange();

  private:
    void updateScoreText();

    WinningHand hand_represented_;
    /*** Widgets used ***/
    QTabWidget *tabs_;
    QWidget *classic_tab_; /**< Tab for classic hand (3 + 3 + 3 + 3 + 2) */
    QWidget *seven_pairs_tab_;
    QWidget *thirteen_orphans_tab_;
    /* For classic hand */
    ClassicGroupSelector *first_group_;
    ClassicGroupSelector *second_group_;
    ClassicGroupSelector *third_group_;
    ClassicGroupSelector *fourth_group_;
    DuoGroupSelector *duo_group_;
    /* For seven pairs */
    DuoGroupSelector *seven_pairs_groups_[7];
    /* Other information for computing fu and fans */
    QRadioButton *ron_button_;
    QRadioButton *tsumo_button_;
    QSpinBox *doras_;
    QCheckBox *riichi_button_;
    QCheckBox *ippatsu_button_;
    QLabel *dominant_wind_label_;
    QLabel *player_wind_label_;
    QComboBox *dominant_wind_selector_;
    QComboBox *player_wind_selector_;
    /* Score display */
    QLabel *score_text_;
    /* Buttons */
    QPushButton *confirm_button_;
    QPushButton *cancel_button_;
};