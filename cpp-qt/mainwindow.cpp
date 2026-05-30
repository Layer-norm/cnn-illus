#include "mainwindow.h"
#include "theme.h"
// #include "overviewwidget.h"
#include "convolutionwidget.h"
#include "poolingwidget.h"
#include "activationwidget.h"
#include "overviewwidget.h"
#include <QApplication>
#include <QPalette>
#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("法轩CNN小课堂");
    resize(900, 800);
    setMinimumSize(800, 700);

    auto *central = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Theme toggle bar
    m_themeBar = new QWidget(central);
    m_themeBar->setStyleSheet(g_theme.panelStyle());
    auto *barLayout = new QHBoxLayout(m_themeBar);
    barLayout->setContentsMargins(8, 4, 8, 4);
    barLayout->addStretch();

    m_themeBtn = new QPushButton(g_isDark ? "☀️ 浅色模式" : "🌙 深色模式", m_themeBar);
    m_themeBtn->setStyleSheet(
        "QPushButton { background: transparent; color: " + g_theme.mutedText +
        "; border: 1px solid " + g_theme.border +
        "; border-radius: 4px; padding: 4px 10px; font-size: 11px; }"
        "QPushButton:hover { background-color: " + g_theme.panelBg + "; }"
    );
    connect(m_themeBtn, &QPushButton::clicked, this, &MainWindow::toggleTheme);
    barLayout->addWidget(m_themeBtn);

    mainLayout->addWidget(m_themeBar);

    m_tabWidget = new QTabWidget(central);
    m_tabWidget->setDocumentMode(true);

    m_overview = new OverviewWidget(this);
    m_conv = new ConvolutionWidget(this);
    m_pool = new PoolingWidget(this);
    m_activation = new ActivationWidget(this);

    m_tabWidget->addTab(m_overview, QString::fromUtf8("🏗️ 网络框架"));
    m_tabWidget->addTab(m_conv, QString::fromUtf8("🔲 卷积层"));
    m_tabWidget->addTab(m_pool, QString::fromUtf8("📊 池化层"));
    m_tabWidget->addTab(m_activation, QString::fromUtf8("⚡ 激活函数"));

    mainLayout->addWidget(m_tabWidget);

    setCentralWidget(central);
}

void MainWindow::toggleTheme()
{
    ::toggleTheme();  // flip global state

    // Apply palette
    QPalette p;
    p.setColor(QPalette::Window, QColor(g_theme.windowBg));
    p.setColor(QPalette::WindowText, QColor(g_theme.text));
    p.setColor(QPalette::Base, QColor(g_theme.baseBg));
    p.setColor(QPalette::AlternateBase, QColor(g_theme.panelBg));
    p.setColor(QPalette::ToolTipBase, QColor(g_theme.border));
    p.setColor(QPalette::ToolTipText, QColor(g_theme.text));
    p.setColor(QPalette::Text, QColor(g_theme.text));
    p.setColor(QPalette::Button, QColor(g_theme.border));
    p.setColor(QPalette::ButtonText, QColor(g_theme.text));
    p.setColor(QPalette::BrightText, QColor("#ef4444"));
    p.setColor(QPalette::Link, QColor(g_theme.accent));
    p.setColor(QPalette::Highlight, QColor(g_theme.accent));
    p.setColor(QPalette::HighlightedText, QColor("#ffffff"));
    qApp->setPalette(p);

    // Update theme bar
    m_themeBar->setStyleSheet(g_theme.panelStyle());
    m_themeBtn->setText(g_isDark ? "☀️ 浅色模式" : "🌙 深色模式");
    m_themeBtn->setStyleSheet(
        "QPushButton { background: transparent; color: " + g_theme.mutedText +
        "; border: 1px solid " + g_theme.border +
        "; border-radius: 4px; padding: 4px 10px; font-size: 11px; }"
        "QPushButton:hover { background-color: " + g_theme.panelBg + "; }"
    );

    // Apply theme to each tab
    m_overview->applyTheme();
    m_conv->applyTheme();
    m_pool->applyTheme();
    m_activation->applyTheme();
}
