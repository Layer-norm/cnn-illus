#ifndef ACTIVATIONWIDGET_H
#define ACTIVATIONWIDGET_H

#include <QWidget>
#include <QTabWidget>
#include <QPainter>
#include <QPainterPath>

class ActivationCanvas : public QWidget
{
    Q_OBJECT
public:
    using Func = double(*)(double);
    ActivationCanvas(Func func, const QString &label, const QColor &color, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Func m_func;
    QString m_label;
    QColor m_color;
};

class ActivationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ActivationWidget(QWidget *parent = nullptr);

private:
    static double relu(double x);
    static double sigmoid(double x);
    static double tanh(double x);
    static double leakyRelu(double x);
};

#endif // ACTIVATIONWIDGET_H
