#pragma once

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QWidget>
#include <array>

#include "tile.hpp"

class TileSelector : public QWidget {
    Q_OBJECT
  public:
    TileSelector(QWidget *parent, bool chii_selector = true);

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
    ClassicGroup(QWidget *parent);

    bool isChi() const;
    bool isPon() const;
    bool isKan() const;
    bool isMelded() const;
    Tile firstTile() const;

  private slots:
    void typeChanged();

  private:
    QRadioButton *chi_;
    QRadioButton *pon_;
    QRadioButton *kan_;
    TileSelector *first_tile_;
    QCheckBox *melded_;
};

class DuoGroup : public QGroupBox {
    Q_OBJECT
  public:
    DuoGroup(QWidget *parent);

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
    HandDialog(QWidget *parent);

  private slots:
    /**
     * @brief Slot called when the hand type is changed
     *
     */
    // void handTypeChanged();

  private:
    /*** Widgets used ***/
    QComboBox *hand_type; /**< Hand type selector (Classic, Seven pairs,
                          Thirteen orphans) */
    /* For classic hand */
    ClassicGroup *first_group;
    ClassicGroup *second_group;
    ClassicGroup *third_group;
    ClassicGroup *fourth_group;
    DuoGroup *duo_group;
    /* For seven pairs */
    DuoGroup *seven_pairs_groups[7];
};