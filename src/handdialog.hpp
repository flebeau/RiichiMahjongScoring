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
#include <qcheckbox.h>
#include <qradiobutton.h>

#include "tile.hpp"

class TileSelector : public QWidget {
    Q_OBJECT
  public:
    TileSelector(QWidget *parent = nullptr, bool chii_selector = true);

    Tile value() const;

  public slots:
    void setChii(bool chii);

  private slots:
    void suitChanged();

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

  signals:
    void typeChanged(bool chii);

  private slots:
    void onTypeChanged();

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
    HandDialog(QWidget *parent = nullptr);

  private:
    /*** Widgets used ***/
    QTabWidget *tabs_;
    QWidget *classic_tab_; /**< Tab for classic hand (3 + 3 + 3 + 3 + 2) */
    QWidget *seven_pairs_tab_;
    QWidget *thirteen_orphans_tab_;
    /* For classic hand */
    ClassicGroupSelector *first_group;
    ClassicGroupSelector *second_group;
    ClassicGroupSelector *third_group;
    ClassicGroupSelector *fourth_group;
    DuoGroupSelector *duo_group;
    /* For seven pairs */
    DuoGroupSelector *seven_pairs_groups_[7];
    /* Other information for computing fu and fans */
    QRadioButton *ron_button_;
    QRadioButton *tsumo_button_;
    QSpinBox *doras_;
    QCheckBox *riichi_button_;
    QCheckBox *ippatsu_button_;
};