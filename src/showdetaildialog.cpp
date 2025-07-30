#include <QDebug>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <qgroupbox.h>

#include "showdetaildialog.hpp"

ShowDetailDialog::ShowDetailDialog(QWidget *parent,
                                   const TurnResult &turn_result,
                                   const ScoreModel::N_Players &n_players,
                                   const std::vector<QString> &player_names)
    : QDialog(parent), label_info_(new QLabel),
      ok_button_(new QPushButton(tr("Ok"))) {
    /* Init winning info */
    QString label_content =
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
        label_content += tr("<b>%1</b> was Riichi<br>").arg(player_names.at(0));
        at_least_one_player_riichi = true;
    }
    if (turn_result.riichiPlayer2()) {
        label_content += tr("<b>%1</b> was Riichi<br>").arg(player_names.at(1));
        at_least_one_player_riichi = true;
    }
    if (turn_result.riichiPlayer3()) {
        label_content += tr("<b>%1</b> was Riichi<br>").arg(player_names.at(2));
        at_least_one_player_riichi = true;
    }
    if (n_players == ScoreModel::N_Players::FOUR_PLAYERS &&
        turn_result.riichiPlayer4()) {
        label_content += tr("<b>%1</b> was Riichi<br>").arg(player_names.at(3));
        at_least_one_player_riichi = true;
    }
    if (at_least_one_player_riichi) {
        label_content += "<br>";
    }

    /* Init fu and fan info */
    label_content += tr("<b>%1</b>").arg(player_names[turn_result.winner()]) +
                     tr(" did a combination valued <b>%1</b> fu and ")
                         .arg(turn_result.fuScore()) +
                     tr("<b>%2</b> fan<br>").arg(turn_result.fanScore());

    /* Init score info */
    std::vector<int> score_change =
        turn_result.computeScoreChange(static_cast<int>(n_players));
    QGroupBox *hand_details = nullptr;
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
        if (turn_result.hand() != nullptr) {
            hand_details = new QGroupBox(tr("Hand details"));
            QLabel *hand_score =
                new QLabel(turn_result.hand()->computeScore().toString());

            QLabel *hand_draw = new QLabel(turn_result.hand()->toUTF8Symbols());
            hand_draw->setAlignment(Qt::AlignCenter);
            hand_draw->setStyleSheet("font-size: 48pt");
            QVBoxLayout *hand_layout = new QVBoxLayout;
            hand_layout->addWidget(hand_draw);
            hand_layout->addWidget(hand_score);
            hand_details->setLayout(hand_layout);
        }
    }

    /* Set label text */
    label_info_->setText(label_content);

    /* Connect ok button with closing of dialog */
    connect(ok_button_, &QPushButton::pressed, this, &ShowDetailDialog::accept);

    /* Main Layout of the dialog  */
    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->addWidget(label_info_);
    if (hand_details != nullptr) {
        main_layout->addWidget(hand_details);
    }
    main_layout->addWidget(ok_button_);

    setLayout(main_layout);

    // Set dialog title
    setWindowTitle("Turn result detail");
}
