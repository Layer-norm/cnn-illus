#include "activationwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <cmath>

ActivationCanvas::ActivationCanvas(Func func, const QString &label, const QColor &color, QWidget *parent)
    : QWidget(parent), m_func(func), m_label(label), m_color(color)
{
    setMinimumSize(650, 200);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void ActivationCanvas::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int W = width();
    int H = height();
    int pad = 40;

    // Background
    painter.fillRect(rect(), QColor("#111827"));

    // Axes
    painter.setPen(QPen(QColor("#64748b"), 1));
    painter.drawLine(pad, H / 2, W - pad, H / 2);
    painter.drawLine(W / 2, pad, W / 2, H - pad);

    // Grid lines
    painter.setPen(QPen(QColor("#1a2035"), 1));
    for (int i = -4; i <= 4; ++i) {
        if (i == 0) continue;
        int x = W / 2 + i * (W - 2 * pad) / 8;
        painter.drawLine(x, pad, x, H - pad);
        int y = H / 2 + i * (H - 2 * pad) / 8;
        painter.drawLine(pad, y, W - pad, y);
    }

    // Tick labels
    QFont tickFont("Consolas", 8);
    painter.setFont(tickFont);
    painter.setPen(QColor("#64748b"));
    for (int i = -4; i <= 4; ++i) {
        if (i == 0) continue;
        int x = W / 2 + i * (W - 2 * pad) / 8;
        painter.drawText(x - 10, H / 2 + 14, 20, 12, Qt::AlignCenter, QString::number(i));
        int y = H / 2 - i * (H - 2 * pad) / 8;
        painter.drawText(W / 2 - 22, y - 6, 20, 12, Qt::AlignRight | Qt::AlignVCenter, QString::number(i));
    }
    painter.drawText(W / 2 - 10, H / 2 + 14, "0");

    // Draw curve
    double xRange = 6.0;
    int steps = 300;
    QPainterPath path;
    for (int i = 0; i <= steps; ++i) {
        double xVal = -xRange / 2.0 + (xRange / steps) * i;
        double yVal = m_func(xVal);
        double px = pad + ((xVal + xRange / 2.0) / xRange) * (W - 2 * pad);
        double py = H - pad - ((yVal + 3.0) / 6.0) * (H - 2 * pad);
        if (i == 0) path.moveTo(px, py);
        else path.lineTo(px, py);
    }

    painter.setPen(QPen(m_color, 2.5));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(path);

    // Label
    QFont labelFont("Consolas", 10, QFont::Bold);
    painter.setFont(labelFont);
    painter.setPen(m_color);
    painter.drawText(pad + 5, pad + 5, W - 2 * pad - 10, 20, Qt::AlignLeft | Qt::AlignVCenter, m_label);
}

// ── Activation functions ──────────────────────────

double ActivationWidget::relu(double x) { return std::max(0.0, x); }
double ActivationWidget::sigmoid(double x) {
    if (x < -50) return 0.0;
    if (x > 50) return 1.0;
    return 1.0 / (1.0 + std::exp(-x));
}
double ActivationWidget::tanh(double x) { return std::tanh(x); }
double ActivationWidget::leakyRelu(double x) { return x > 0 ? x : 0.01 * x; }

ActivationWidget::ActivationWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(12);

    auto *title = new QLabel(QString::fromUtf8("⚡ 激活函数 Activation Functions"));
    title->setStyleSheet("font-size: 15px; font-weight: bold; color: #e2e8f0;");
    layout->addWidget(title);

    struct ActInfo {
        QString name;
        ActivationCanvas::Func func;
        QColor color;
    };

    QVector<ActInfo> acts = {
        {"ReLU: max(0, x)", relu, QColor("#3b82f6")},
        {"Sigmoid: σ(x) = 1/(1+e⁻ˣ)", sigmoid, QColor("#10b981")},
        {"Tanh: tanh(x)", tanh, QColor("#f59e0b")},
        {"Leaky ReLU (α=0.01)", leakyRelu, QColor("#ec4899")},
    };

    for (const auto &act : acts) {
        auto *frame = new QFrame(this);
        frame->setStyleSheet("QFrame { background-color: #111827; border: 1px solid #232b42; border-radius: 8px; }");
        auto *fl = new QVBoxLayout(frame);
        fl->setContentsMargins(8, 8, 8, 8);

        auto *canvas = new ActivationCanvas(act.func, act.name, act.color, frame);
        fl->addWidget(canvas);

        layout->addWidget(frame);
    }

    // Formula summary
    {
        auto *frame = new QFrame(this);
        frame->setStyleSheet("QFrame { background-color: #1a2035; border: 1px solid #232b42; border-radius: 8px; }");
        auto *fl = new QVBoxLayout(frame);
        fl->setContentsMargins(15, 10, 15, 10);

        auto addRow = [&](const QString &name, const QString &formula, const QString &color) {
            auto *row = new QWidget(frame);
            auto *rl = new QHBoxLayout(row);
            rl->setContentsMargins(0, 2, 0, 2);

            auto *nl = new QLabel(name, row);
            nl->setStyleSheet(QString("font-size: 11px; font-weight: bold; color: %1;").arg(color));
            nl->setFixedWidth(120);
            rl->addWidget(nl);

            auto *fl2 = new QLabel(formula, row);
            fl2->setStyleSheet(QString("font-size: 12px; font-weight: bold; color: %1;").arg(color));
            rl->addWidget(fl2);

            rl->addStretch();
            fl->addWidget(row);
        };

        addRow("ReLU", "f(x) = max(0, x)", "#3b82f6");
        addRow("Sigmoid", "f(x) = 1 / (1 + e⁻ˣ)", "#10b981");
        addRow("Tanh", "f(x) = (eˣ − e⁻ˣ) / (eˣ + e⁻ˣ)", "#f59e0b");
        addRow("Leaky ReLU", "f(x) = x if x>0, αx if x≤0", "#ec4899");

        layout->addWidget(frame);
    }

    layout->addStretch();
}
