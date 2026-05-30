#ifndef ACTIVATIONWIDGET_H
#define ACTIVATIONWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QPainter>
#include <QPainterPath>
#include <QVector>
#include <QPoint>
#include <QMouseEvent>
#include <QWheelEvent>

struct ActivationFunc {
    QString name;
    QString formula;
    double (*func)(double);
    QColor color;
    bool visible = true;
};

class ActivationCanvas : public QWidget
{
    Q_OBJECT
public:
    explicit ActivationCanvas(QWidget *parent = nullptr);
    void setFunctions(const QVector<ActivationFunc> *funcs) { m_funcs = funcs; update(); }
    void setThemeColors(const QColor &bg, const QColor &grid, const QColor &axis, const QColor &tick);

public slots:
    void resetView();

protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    const QVector<ActivationFunc> *m_funcs = nullptr;
    double m_centerX = 0.0, m_centerY = 0.0;
    double m_scale = 70.0;  // pixels per unit
    QPoint m_lastPos;
    bool m_dragging = false;
    QColor m_bgColor{"#111827"};
    QColor m_gridColor{"#1e293b"};
    QColor m_axisColor{"#64748b"};
    QColor m_tickColor{"#64748b"};
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
    static double silu(double x);
    static double elu(double x);

    QLabel *m_title;
    ActivationCanvas *m_canvas;
    QVector<ActivationFunc> m_funcs;
    QVector<QCheckBox*> m_checkBoxes;

    void updateDisplay();
};

#endif // ACTIVATIONWIDGET_H
