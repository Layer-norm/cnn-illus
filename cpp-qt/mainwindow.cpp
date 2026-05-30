#include "mainwindow.h"
#include "overviewwidget.h"
#include "convolutionwidget.h"
#include "poolingwidget.h"
#include "activationwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("CNN 卷积神经网络原理交互演示");
    resize(900, 800);
    setMinimumSize(800, 700);

    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setDocumentMode(true);

    auto *overview = new OverviewWidget(this);
    auto *conv = new ConvolutionWidget(this);
    auto *pool = new PoolingWidget(this);
    auto *activation = new ActivationWidget(this);

    m_tabWidget->addTab(overview, QString::fromUtf8("🏗️ 网络框架"));
    m_tabWidget->addTab(conv, QString::fromUtf8("🔲 卷积层"));
    m_tabWidget->addTab(pool, QString::fromUtf8("📊 池化层"));
    m_tabWidget->addTab(activation, QString::fromUtf8("⚡ 激活函数"));

    setCentralWidget(m_tabWidget);
}
