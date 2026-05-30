#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QPushButton>

class OverviewWidget;
class ConvolutionWidget;
class PoolingWidget;
class ActivationWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void toggleTheme();

private:
    QTabWidget *m_tabWidget;
    QPushButton *m_themeBtn;
    QWidget *m_themeBar;
    OverviewWidget *m_overview;
    ConvolutionWidget *m_conv;
    PoolingWidget *m_pool;
    ActivationWidget *m_activation;
};

#endif // MAINWINDOW_H
