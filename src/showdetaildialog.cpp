#include <QApplication>
#include <QDebug>
#include <QFontMetrics>
#include <QGraphicsProxyWidget>
#include <QGraphicsTextItem>
#include <QHBoxLayout>
#include <QPair>
#include <QTransform>
#include <QVBoxLayout>
#include <qdebug.h>
#include <qglobal.h>
#include <qgraphicsitem.h>
#include <qgroupbox.h>
#include <qnamespace.h>
#include <qsizepolicy.h>

#include "showdetaildialog.hpp"

ShowDetailDialog::ShowDetailDialog(QWidget *parent,
                                   const TurnResult &turn_result,
                                   const ScoreModel::N_Players &n_players,
                                   const std::vector<QString> &player_names)
    : QDialog(parent), label_info_(new QLabel),
      ok_button_(new QPushButton(tr("Ok"))) {

    /* Main Layout of the dialog  */
    QVBoxLayout *main_layout = new QVBoxLayout;

    /* Init winning info */
    QString label_content;

    if (turn_result.isDraw()) {
        label_content += tr("The round ended in a draw<br><br>");
        const std::vector<bool> &players_tenpai = turn_result.playersTenpai();
        for (int i = 0;
             i < (n_players == ScoreModel::N_Players::FOUR_PLAYERS ? 4 : 3);
             ++i) {
            label_content += tr("<b>%1</b> was <b>%2</b><br><br>")
                                 .arg(player_names[i],
                                      players_tenpai[i] ? "tenpai" : "noten");
        }
    } else if (!turn_result.isManualScore()) {
        label_content +=
            tr("<b>%1</b> won the round by <b>%2</b> victory%3<br><br>")
                .arg(player_names[turn_result.winner()],
                     turn_result.ronVictory() ? "ron" : "tsumo",
                     turn_result.ronVictory()
                         ? tr(" (loser: <b>%1</b>)")
                               .arg(player_names.at(turn_result.loser()))
                         : "");

        /* Init East info */
        label_content += tr("<b>%1</b> was East<br><br>")
                             .arg(player_names.at(turn_result.eastPlayer()));

        /* Init riichi info */
        bool at_least_one_player_riichi = false;
        if (turn_result.riichiPlayer1()) {
            label_content +=
                tr("<b>%1</b> was Riichi<br>").arg(player_names.at(0));
            at_least_one_player_riichi = true;
        }
        if (turn_result.riichiPlayer2()) {
            label_content +=
                tr("<b>%1</b> was Riichi<br>").arg(player_names.at(1));
            at_least_one_player_riichi = true;
        }
        if (turn_result.riichiPlayer3()) {
            label_content +=
                tr("<b>%1</b> was Riichi<br>").arg(player_names.at(2));
            at_least_one_player_riichi = true;
        }
        if (n_players == ScoreModel::N_Players::FOUR_PLAYERS &&
            turn_result.riichiPlayer4()) {
            label_content +=
                tr("<b>%1</b> was Riichi<br>").arg(player_names.at(3));
            at_least_one_player_riichi = true;
        }
        if (at_least_one_player_riichi) {
            label_content += "<br>";
        }

        /* Init fu and fan info */
        label_content +=
            tr("<b>%1</b>").arg(player_names[turn_result.winner()]) +
            tr(" did a combination valued <b>%1</b> fu and ")
                .arg(turn_result.fuScore()) +
            tr("<b>%2</b> fan<br>").arg(turn_result.fanScore());
    }

    /* Init score info */
    std::vector<int> score_change =
        turn_result.computeScoreChange(static_cast<int>(n_players));
    // If any score change, init the tabular
    if (score_change[0] != 0 || score_change[1] != 0 || score_change[2] != 0 ||
        (n_players == ScoreModel::N_Players::FOUR_PLAYERS &&
         score_change[3] != 0)) {
        label_content += "<table>";
        if (score_change[0] != 0) {
            label_content +=
                tr("<tr><td><b>%1</b></td> %2 points</td></tr>")
                    .arg(player_names[0],
                         score_change[0] > 0
                             ? tr("<td>won</td> <td align=\"right\">%1")
                                   .arg(score_change[0])
                             : tr("<td>lost</td> <td align=\"right\">%1")
                                   .arg(-score_change[0]));
        }
        if (score_change[1] != 0) {
            label_content +=
                tr("<tr><td><b>%1</b></td> %2 points</td></tr>")
                    .arg(player_names[1],
                         score_change[1] > 0
                             ? tr("<td>won</td> <td align=\"right\">%1")
                                   .arg(score_change[1])
                             : tr("<td>lost</td> <td align=\"right\">%1")
                                   .arg(-score_change[1]));
        }
        if (score_change[2] != 2) {
            label_content +=
                tr("<tr><td><b>%1</b></td> %2 points</td></tr>")
                    .arg(player_names[2],
                         score_change[2] > 0
                             ? tr("<td>won</td> <td align=\"right\">%1")
                                   .arg(score_change[2])
                             : tr("<td>lost</td> <td align=\"right\">%1")
                                   .arg(-score_change[2]));
        }
        if (n_players == ScoreModel::N_Players::FOUR_PLAYERS &&
            score_change[3] != 0) {
            label_content +=
                tr("<tr><td><b>%1</b></td> %2 points</td></tr>")
                    .arg(player_names[3],
                         score_change[3] > 0
                             ? tr("<td>won</td> <td align=\"right\">%1")
                                   .arg(score_change[3])
                             : tr("<td>lost</td> <td align=\"right\">%1")
                                   .arg(-score_change[3]));
        }
        label_content += "</table>\n";
    }
    /* Set label text */
    label_info_->setText(label_content);
    main_layout->addWidget(label_info_);

    if (turn_result.hand() != nullptr) {
        QGroupBox *hand_details = new QGroupBox(tr("Hand details"));
        QLabel *hand_score =
            new QLabel(turn_result.hand()->computeScore().toString());

        hand_scene_ = new QGraphicsScene();
        hand_view_ = new QGraphicsView(hand_scene_);
        hand_view_->setAlignment(Qt::AlignCenter);
        hand_view_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        hand_view_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        QString utf8Hand = turn_result.hand()->toUTF8Symbols();

        int tileHeight = 48;

        QFont font;
        font.setPointSize(tileHeight);
        font.setFamily("Noto Sans, Arial, DejaVu Sans");

        int tileWidth = 49;
        int x = 0;

        for (auto it = utf8Hand.constBegin(); it != utf8Hand.constEnd(); ++it) {
            if (it->isHighSurrogate() && (it + 1) != utf8Hand.constEnd() &&
                (it + 1)->isLowSurrogate()) {

                QString tileStr = QString(it, 2);
                QGraphicsTextItem *tile = new QGraphicsTextItem(tileStr);
                tile->setFont(font);
                tile->setPos(x, 0);
                hand_scene_->addItem(tile);
                x += tileWidth;
                ++it;
            } else if (*it == '#') {
                if (!hand_scene_->items().isEmpty()) {
                    QGraphicsTextItem *lastTile =
                        dynamic_cast<QGraphicsTextItem *>(
                            hand_scene_->items().first());
                    if (lastTile) {
                        lastTile->setTransformOriginPoint(53, 84);
                        lastTile->setRotation(90);
                        lastTile->setPos(x - 95, 1);
                        x = x - tileWidth + 60;
                    }
                }
            } else if (*it == ' ') {
                x += tileWidth / 4;
            }
        }

        hand_scene_->setSceneRect(-3, 14, x + 14, 82);
        hand_view_->fitInView(hand_scene_->sceneRect(), Qt::KeepAspectRatio);
        hand_view_->setMinimumHeight(80);

        QVBoxLayout *hand_layout = new QVBoxLayout;
        hand_layout->addWidget(hand_view_);
        hand_layout->addWidget(hand_score);
        hand_details->setLayout(hand_layout);
        main_layout->addWidget(hand_details);
    }

    /* Connect ok button with closing of dialog */
    connect(ok_button_, &QPushButton::pressed, this, &ShowDetailDialog::accept);

    main_layout->addWidget(ok_button_);

    setLayout(main_layout);

    // Set dialog title
    setWindowTitle("Turn result detail");
}

void ShowDetailDialog::resizeEvent(QResizeEvent *event) {
    QDialog::resizeEvent(event);
    if (hand_view_ != nullptr && hand_scene_ != nullptr) {
        hand_view_->fitInView(hand_scene_->sceneRect(), Qt::KeepAspectRatio);
    }
}