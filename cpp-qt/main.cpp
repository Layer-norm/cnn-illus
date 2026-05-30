#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("CNN Visualizer");
    app.setStyle("Fusion");

    // Dark theme palette
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor("#0a0e1a"));
    darkPalette.setColor(QPalette::WindowText, QColor("#e2e8f0"));
    darkPalette.setColor(QPalette::Base, QColor("#111827"));
    darkPalette.setColor(QPalette::AlternateBase, QColor("#1a2035"));
    darkPalette.setColor(QPalette::ToolTipBase, QColor("#232b42"));
    darkPalette.setColor(QPalette::ToolTipText, QColor("#e2e8f0"));
    darkPalette.setColor(QPalette::Text, QColor("#e2e8f0"));
    darkPalette.setColor(QPalette::Button, QColor("#232b42"));
    darkPalette.setColor(QPalette::ButtonText, QColor("#e2e8f0"));
    darkPalette.setColor(QPalette::BrightText, QColor("#ef4444"));
    darkPalette.setColor(QPalette::Link, QColor("#3b82f6"));
    darkPalette.setColor(QPalette::Highlight, QColor("#3b82f6"));
    darkPalette.setColor(QPalette::HighlightedText, QColor("#ffffff"));
    app.setPalette(darkPalette);

    MainWindow window;
    window.show();
    return app.exec();
}
