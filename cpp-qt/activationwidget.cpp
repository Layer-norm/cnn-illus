#include "activationwidget.h"
#include "theme.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QFont>
#include <cmath>

// ── Activation functions ──────────────────────────

double ActivationWidget::relu(double x) { return std::max(0.0, x); }
double ActivationWidget::sigmoid(double x) {
    if (x < -50) return 0.0;
    if (x > 50) return 1.0;
    return 1.0 / (1.0 + std::exp(-x));
}
double ActivationWidget::tanh(double x) { return std::tanh(x); }
double ActivationWidget::leakyRelu(double x) { return x > 0 ? x : 0.01 * x; }
double ActivationWidget::silu(double x) {
    // SiLU(x) = x * sigmoid(x)
    if (x < -50) return 0.0;
    if (x > 50) return x;
    return x / (1.0 + std::exp(-x));
}
double ActivationWidget::elu(double x) {
    // ELU(x) = x if x > 0, alpha*(exp(x)-1) if x <= 0, alpha = 1.0
    return x > 0 ? x : std::exp(x) - 1.0;
}

// ── ActivationCanvas ──────────────────────────────

ActivationCanvas::ActivationCanvas(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(400, 300);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMouseTracking(true);
    setCursor(Qt::OpenHandCursor);
}

void ActivationCanvas::setThemeColors(const QColor &bg, const QColor &grid, const QColor &axis, const QColor &tick)
{
    m_bgColor = bg;
    m_gridColor = grid;
    m_axisColor = axis;
    m_tickColor = tick;
    update();
}

void ActivationCanvas::resetView()
{
    m_centerX = 0.0;
    m_centerY = 0.0;
    m_scale = 70.0;
    update();
}

void ActivationCanvas::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int W = width();
    int H = height();

    // Background
    painter.fillRect(rect(), m_bgColor);

    // Visible world range
    double xMin = m_centerX - W / (2.0 * m_scale);
    double xMax = m_centerX + W / (2.0 * m_scale);
    double yMin = m_centerY - H / (2.0 * m_scale);
    double yMax = m_centerY + H / (2.0 * m_scale);

    // Helper lambdas: world ⇔ screen
    auto wx = [&](double x) -> double { return W/2.0 + (x - m_centerX) * m_scale; };
    auto wy = [&](double y) -> double { return H/2.0 - (y - m_centerY) * m_scale; };

    // ── Grid ──
    // Adaptive grid step
    double xRange = xMax - xMin;
    double yRange = yMax - yMin;

    auto adaptiveStep = [](double range) -> double {
        if (range > 20.0) return 5.0;
        if (range > 10.0) return 2.0;
        if (range > 4.0)  return 1.0;
        if (range > 1.5)  return 0.5;
        return 0.25;
    };
    double xStep = adaptiveStep(xRange);
    double yStep = adaptiveStep(yRange);

    QFont gridFont("Consolas", 8);
    painter.setFont(gridFont);

    // Vertical grid lines + x tick labels
    painter.setPen(QPen(m_gridColor, 1));
    int xi0 = int(std::ceil(xMin / xStep));
    for (int i = xi0; i <= std::floor(xMax / xStep); ++i) {
        double xVal = i * xStep;
        if (std::abs(xVal) < 1e-9) continue;

        int px = int(wx(xVal));
        painter.drawLine(px, 0, px, H);

        painter.setPen(m_tickColor);
        painter.drawText(px - 20, int(wy(0)) + 4, 40, 12, Qt::AlignCenter,
                         QString::number(xVal, 'f', xStep < 1.0 ? 1 : 0));
        painter.setPen(QPen(m_gridColor, 1));
    }

    // Horizontal grid lines + y tick labels
    painter.setPen(QPen(m_gridColor, 1));
    int yi0 = int(std::ceil(yMin / yStep));
    for (int i = yi0; i <= std::floor(yMax / yStep); ++i) {
        double yVal = i * yStep;
        if (std::abs(yVal) < 1e-9) continue;

        int py = int(wy(yVal));
        painter.drawLine(0, py, W, py);

        painter.setPen(m_tickColor);
        painter.drawText(int(wx(0)) + 5, py - 6, 40, 12, Qt::AlignLeft | Qt::AlignVCenter,
                         QString::number(yVal, 'f', yStep < 1.0 ? 1 : 0));
        painter.setPen(QPen(m_gridColor, 1));
    }

    // ── Axes (x=0, y=0) ──
    painter.setPen(QPen(m_axisColor, 1.5));
    int zeroX = int(wx(0));
    int zeroY = int(wy(0));
    if (zeroX >= 0 && zeroX <= W) painter.drawLine(zeroX, 0, zeroX, H);
    if (zeroY >= 0 && zeroY <= H) painter.drawLine(0, zeroY, W, zeroY);

    // Origin label
    painter.setPen(m_tickColor);
    painter.drawText(zeroX - 10, zeroY + 14, 20, 14, Qt::AlignCenter, "0");

    // ── Function curves ──
    if (!m_funcs || m_funcs->isEmpty()) return;

    int steps = std::clamp(int(xRange * m_scale * 0.4), 100, 1200);

    painter.setRenderHint(QPainter::Antialiasing, true);
    for (const auto &f : *m_funcs) {
        if (!f.visible) continue;

        QPainterPath path;
        bool hasPoints = false;
        bool wasClamped = true;

        for (int i = 0; i <= steps; ++i) {
            double xVal = xMin + (xRange / steps) * i;
            double yVal = f.func(xVal);
            bool clamped = (yVal < yMin - 2.0 || yVal > yMax + 2.0);

            if (!clamped) {
                double px = wx(xVal);
                double py = wy(yVal);
                if (wasClamped || !hasPoints) {
                    path.moveTo(px, py);
                    hasPoints = true;
                } else {
                    path.lineTo(px, py);
                }
            }
            wasClamped = clamped;
        }

        painter.setPen(QPen(f.color, 2.5));
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(path);
    }

    // ── Legend with formulas ──
    int lx = 12;
    int ly = 12;
    QFont legendFont("Consolas", 9, QFont::Bold);
    QFont formulaFont("Consolas", 8);
    painter.setFont(legendFont);
    for (const auto &f : *m_funcs) {
        if (!f.visible) continue;
        // Color block
        painter.fillRect(lx, ly, 14, 14, f.color);
        // Function name
        painter.setPen(m_tickColor);
        painter.drawText(lx + 20, ly - 1, 150, 16, Qt::AlignLeft | Qt::AlignVCenter, f.name);
        // Formula below the name
        painter.setFont(formulaFont);
        int formulaY = ly + 18;
        painter.drawText(lx + 20, formulaY - 1, 350, 14, Qt::AlignLeft | Qt::AlignVCenter, f.formula);
        ly += 38;  // total height per entry: 18 (name line) + 14 (formula) + 6 gap
        painter.setFont(legendFont);
    }
}

void ActivationCanvas::wheelEvent(QWheelEvent *event)
{
    double factor = event->angleDelta().y() > 0 ? 1.12 : 1.0 / 1.12;

    // Zoom around cursor position
    QPointF pos = event->position();
    double worldX = m_centerX + (pos.x() - width() / 2.0) / m_scale;
    double worldY = m_centerY - (pos.y() - height() / 2.0) / m_scale;

    m_scale = std::clamp(m_scale * factor, 8.0, 600.0);

    // Adjust center so world point stays under cursor
    m_centerX = worldX - (pos.x() - width() / 2.0) / m_scale;
    m_centerY = worldY - (pos.y() - height() / 2.0) / m_scale;

    update();
}

void ActivationCanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_lastPos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
}

void ActivationCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        setCursor(Qt::OpenHandCursor);
    }
}

void ActivationCanvas::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging) {
        QPoint delta = event->pos() - m_lastPos;
        m_centerX -= delta.x() / m_scale;
        m_centerY += delta.y() / m_scale;
        m_lastPos = event->pos();
        update();
    }
}

void ActivationCanvas::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        resetView();
    }
}

// ── ActivationWidget ──────────────────────────────

ActivationWidget::ActivationWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(10);

    // Title
    m_title = new QLabel(QString::fromUtf8("⚡ 激活函数 Activation Functions"));
    m_title->setStyleSheet(g_theme.titleStyle());
    layout->addWidget(m_title);

    // Function definitions (name, formula, func ptr, color, default visible)
    m_funcs = {
        {"Sigmoid",    "f(x) = 1 / (1 + e⁻ˣ)",                     sigmoid,   QColor("#10b981"), true},
        {"Tanh",       "tanh(x) = (eˣ−e⁻ˣ)/(eˣ+e⁻ˣ)",            tanh,      QColor("#f59e0b"), false},
        {"ReLU",       "f(x) = max(0, x)",                         relu,      QColor("#3b82f6"), false},
        {"Leaky ReLU", "f(x) = x (x>0), αx (x≤0, α=0.01)",        leakyRelu, QColor("#ec4899"), false},
        {"ELU",        "f(x) = x (x>0), α(eˣ−1) (x≤0, α=1.0)",    elu,       QColor("#ef4444"), false},
        {"SiLU",       "f(x) = x ⋅ σ(x)",                          silu,      QColor("#8b5cf6"), false},
    };

    // Checkboxes row
    auto *checkLayout = new QHBoxLayout;
    checkLayout->setSpacing(12);
    for (int i = 0; i < m_funcs.size(); ++i) {
        auto *cb = new QCheckBox(m_funcs[i].name, this);
        cb->setChecked(m_funcs[i].visible);
        cb->setStyleSheet(QString("font-size: 11px; font-weight: bold; color: %1;")
                         .arg(m_funcs[i].color.name()));
        connect(cb, &QCheckBox::toggled, this, [this]() { updateDisplay(); });
        checkLayout->addWidget(cb);
        m_checkBoxes.append(cb);
    }
    // Reset view button
    auto *resetBtn = new QPushButton(QString::fromUtf8("↺ 重置视图"), this);
    resetBtn->setStyleSheet(g_theme.btnSecStyle());
    connect(resetBtn, &QPushButton::clicked, this, [this]() { m_canvas->resetView(); });
    checkLayout->addWidget(resetBtn);
    checkLayout->addStretch();
    layout->addLayout(checkLayout);

    // Canvas
    m_canvas = new ActivationCanvas(this);
    m_canvas->setFunctions(&m_funcs);
    layout->addWidget(m_canvas, 1);  // stretch factor 1 = takes remaining space

    // Usage hint
    auto *hint = new QLabel(QString::fromUtf8(
        "💡 滚轮缩放 · 拖拽平移 · 双击重置   |  勾选/取消函数进行对比观察"), this);
    hint->setStyleSheet(QString("font-size: 10px; color: %1;").arg(g_theme.mutedText));
    hint->setAlignment(Qt::AlignCenter);
    layout->addWidget(hint);

    // Initial sync
    updateDisplay();

    // Apply initial theme
    applyTheme();
}

void ActivationWidget::updateDisplay()
{
    // Sync checkbox states to m_funcs visible flags
    for (int i = 0; i < m_funcs.size() && i < m_checkBoxes.size(); ++i) {
        m_funcs[i].visible = m_checkBoxes[i]->isChecked();
    }

    // Refresh canvas
    m_canvas->update();
}

void ActivationWidget::applyTheme()
{
    // Title
    m_title->setStyleSheet(g_theme.titleStyle());

    // Updates canvas theme colors
    m_canvas->setThemeColors(
        QColor(g_theme.baseBg),
        QColor(g_theme.border),
        QColor(g_theme.mutedText),
        QColor(g_theme.mutedText)
    );
}
