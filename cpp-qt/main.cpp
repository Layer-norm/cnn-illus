#include <QApplication>
#include <QPalette>
#include "mainwindow.h"
#include "theme.h"

static void applyPalette()
{
    auto &t = g_theme;
    QPalette p;
    p.setColor(QPalette::Window, QColor(t.windowBg));
    p.setColor(QPalette::WindowText, QColor(t.text));
    p.setColor(QPalette::Base, QColor(t.baseBg));
    p.setColor(QPalette::AlternateBase, QColor(t.panelBg));
    p.setColor(QPalette::ToolTipBase, QColor(t.border));
    p.setColor(QPalette::ToolTipText, QColor(t.text));
    p.setColor(QPalette::Text, QColor(t.text));
    p.setColor(QPalette::Button, QColor(t.border));
    p.setColor(QPalette::ButtonText, QColor(t.text));
    p.setColor(QPalette::BrightText, QColor("#ef4444"));
    p.setColor(QPalette::Link, QColor(t.accent));
    p.setColor(QPalette::Highlight, QColor(t.accent));
    p.setColor(QPalette::HighlightedText, QColor("#ffffff"));
    qApp->setPalette(p);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("CNN Visualizer");
    app.setStyle("Fusion");

    applyPalette();

    MainWindow window;
    window.show();
    return app.exec();
}
