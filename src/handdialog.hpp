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

class ClassicGroup : public QGroupBox {
    Q_OBJECT
  public:
    ClassicGroup(QWidget *parent = nullptr);

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

class DuoGroup : public QGroupBox {
    Q_OBJECT
  public:
    DuoGroup(QWidget *parent = nullptr);

    bool isMelded() const;
    Tile tile() const;

  private:
    TileSelector *tile_selector_;
    QCheckBox *melded_;
};

/**
 * @brief Implements the dialog for adding the winning hand of a turn
 *
 */
class HandDialog : public QDialog {
    Q_OBJECT
  public:
    HandDialog(QWidget *parent = nullptr);

  private slots:
    /**
     * @brief Slot called when the hand type is changed
     *
     */
    // void handTypeChanged();

  private:
    /*** Widgets used ***/
    QTabWidget *tabs_;
    QWidget *classic_tab_; /**< Tab for classic hand (3 + 3 + 3 + 3 + 2) */
    QWidget *seven_pairs_tab_;
    QWidget *thirteen_orphans_tab_;
    /* For classic hand */
    ClassicGroup *first_group;
    ClassicGroup *second_group;
    ClassicGroup *third_group;
    ClassicGroup *fourth_group;
    DuoGroup *duo_group;
    /* For seven pairs */
    DuoGroup *seven_pairs_groups_[7];
};