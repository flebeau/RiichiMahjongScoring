#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QDialog>
#include <QFile>
#include <iostream>

#include "mainwindow.hpp"
#include "scoremodel.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("RiichiMahjongScoring");
    QCoreApplication::setApplicationVersion("0.1");
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();

    // Option for analysing a game
    QCommandLineOption analyze_option(QStringList() << "a" << "analyze",
                                      QDialog::tr("Analyze a scoresheet file."),
                                      "file");
    parser.addOption(analyze_option);

    parser.process(app);

    QString analyze_file = parser.value(analyze_option);
    if (analyze_file != "") {
        ScoreModel score_model(NULL);

        QFile file(analyze_file);
        if (!file.open(QIODevice::ReadOnly)) {
            std::cerr << "Error when opening scoresheet file" << std::endl;
            exit(-1);
        }

        QTextStream in(&file);
        if (!score_model.loadFromTextStream(in)) {
            std::cerr << "Error when parsing scoresheet file" << std::endl;
        }

        QTextStream out(stdout);

        out << static_cast<int>(score_model.NPlayers()) << Qt::endl;
        out << score_model.PlayerNames()[0] << Qt::endl
            << score_model.PlayerNames()[1] << Qt::endl
            << score_model.PlayerNames()[2] << Qt::endl;
        if (score_model.NPlayers() == ScoreModel::N_Players::FOUR_PLAYERS) {
            out << score_model.PlayerNames()[3] << Qt::endl;
        }

        const std::vector<TurnResult> &results = score_model.turnResults();
        for (const auto &result : results) {
            std::vector<int> score_change = result.computeScoreChange(
                static_cast<int>(score_model.NPlayers()));
            out << score_change[0];
            for (size_t i = 1; i < score_change.size(); i++) {
                out << " " << score_change[i];
            }
            out << Qt::endl;
        }

        return 0;
    } else {
        MainWindow main_window;
        main_window.setStyleSheet("QWidget { font-size: 18px }");
        main_window.show();

        return app.exec();
    }
}