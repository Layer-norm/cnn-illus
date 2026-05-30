#ifndef ACTIVATIONWIDGET_H
#define ACTIVATIONWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTabWidget>
#include <QPainter>
#include <QPainterPath>

class ActivationCanvas : public QWidget
{
    Q_OBJECT
public:
    using Func = double(*)(double);
    ActivationCanvas(Func func, const QString &label, const QColor &color, QWidget *parent = nullptr);
    void setBgColor(const QColor &bg) { m_bgColor = bg; update(); }
    void setGridColor(const QColor &c) { m_gridColor = c; update(); }
    void setAxisColor(const QColor &c) { m_axisColor = c; update(); }
    void setTickColor(const QColor &c) { m_tickColor = c; update(); }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Func m_func;
    QString m_label;
    QColor m_color;
    QColor m_bgColor = QColor("#111827");
    QColor m_gridColor = QColor("#1a2035");
    QColor m_axisColor = QColor("#64748b");
    QColor m_tickColor = QColor("#64748b");
};

class ActivationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ActivationWidget(QWidget *parent = nullptr);
    void applyTheme();

private:
    static double relu(double x);
    static double sigmoid(double x);
    static double tanh(double x);
    static double leakyRelu(double x);
    QLabel *m_title;
    QWidget *m_formulaFrame;
    QVector<QWidget*> m_frames;
    QVector<ActivationCanvas*> m_canvases;
};

#endif // ACTIVATIONWIDGET_H
