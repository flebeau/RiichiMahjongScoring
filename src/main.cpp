#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <iostream>

#include "mainwindow.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("RiichiMahjongScoring");
    QCoreApplication::setApplicationVersion("0.1");
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);

    MainWindow main_window;
    main_window.show();

    return app.exec();
}