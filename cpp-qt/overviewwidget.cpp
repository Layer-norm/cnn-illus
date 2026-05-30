#include "overviewwidget.h"
#include "theme.h"
#include <QVBoxLayout>
#include <QLabel>

OverviewWidget::OverviewWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(20, 20, 20, 20);
    outerLayout->setSpacing(15);

    auto *title = new QLabel(QString::fromUtf8("🏗️ CNN 网络总体框架"));
    title->setStyleSheet(g_theme.titleStyle());
    outerLayout->addWidget(title);

    // Architecture canvas
    m_canvas = new QWidget(this);
    m_canvas->setMinimumHeight(140);
    m_canvas->setStyleSheet(QString("background-color: %1; border: 1px solid %2; border-radius: 8px;").arg(g_theme.baseBg).arg(g_theme.border));
    outerLayout->addWidget(m_canvas);

    // Content container for rebuildable content
    m_contentWidget = new QWidget(this);
    outerLayout->addWidget(m_contentWidget);
    outerLayout->addStretch();

    rebuildContent();
}

void OverviewWidget::rebuildContent()
{
    // Clear old
    QLayoutItem *item;
    while ((item = m_contentWidget->layout() ? m_contentWidget->layout()->takeAt(0) : nullptr) != nullptr) {
        delete item->widget();
        delete item;
    }
    delete m_contentWidget->layout();

    auto *layout = new QVBoxLayout(m_contentWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(15);

    // Core concepts
    struct Concept {
        QString title, desc, color;
    };
    QVector<Concept> concepts = {
        {QString::fromUtf8("🔍 局部感受野"),
         QString::fromUtf8("每个神经元只关注输入的一个局部区域，而非全部像素。这大幅减少了参数数量，并保留了空间结构信息。"),
         "#3b82f6"},
        {QString::fromUtf8("🔄 权值共享机制"),
         QString::fromUtf8("同一个卷积核在整个输入上滑动，所有位置共享相同的权重参数，使网络具有平移不变性。"),
         "#10b981"},
        {QString::fromUtf8("📊 层次化特征提取"),
         QString::fromUtf8("浅层学习边缘、纹理等低级特征，深层学习形状、物体等高级语义特征，逐层抽象。"),
         "#8b5cf6"},
    };

    for (const auto &c : concepts) {
        auto *frame = new QWidget(m_contentWidget);
        frame->setStyleSheet(QString("background-color: %1; border: 1px solid %2; border-radius: 8px;").arg(g_theme.baseBg).arg(g_theme.border));
        auto *fl = new QVBoxLayout(frame);
        fl->setContentsMargins(15, 10, 15, 10);

        auto *ct = new QLabel(c.title);
        ct->setStyleSheet(QString("font-size: 13px; font-weight: bold; color: %1;").arg(c.color));
        fl->addWidget(ct);

        auto *cd = new QLabel(c.desc);
        cd->setStyleSheet(QString("font-size: 11px; color: %1;").arg(g_theme.labelText));
        cd->setWordWrap(true);
        fl->addWidget(cd);

        layout->addWidget(frame);
    }

    // Formula
    auto *formulaFrame = new QWidget(m_contentWidget);
    formulaFrame->setStyleSheet(QString("background-color: %1; border: 1px solid %2; border-radius: 8px;").arg(g_theme.panelBg).arg(g_theme.border));
    auto *formulaLayout = new QVBoxLayout(formulaFrame);
    formulaLayout->setContentsMargins(15, 12, 15, 12);

    auto *formulaTitle = new QLabel(QString::fromUtf8("📐 输出尺寸计算公式"));
    formulaTitle->setStyleSheet(QString("font-size: 13px; font-weight: bold; color: %1;").arg(g_theme.cyan));
    formulaLayout->addWidget(formulaTitle);

    auto *formula = new QLabel(QString::fromUtf8("O = ⌊(I − K + 2P) / S⌋ + 1"));
    formula->setStyleSheet(QString("font-size: 16px; font-weight: bold; color: %1; padding: 8px 0;").arg(g_theme.cyan));
    formula->setAlignment(Qt::AlignCenter);
    formulaLayout->addWidget(formula);

    auto *desc = new QLabel(QString::fromUtf8("O = 输出尺寸  I = 输入尺寸  K = 卷积核/池化窗口大小  P = 填充大小  S = 步幅"));
    desc->setStyleSheet(QString("font-size: 11px; color: %1;").arg(g_theme.mutedText));
    desc->setAlignment(Qt::AlignCenter);
    desc->setWordWrap(true);
    formulaLayout->addWidget(desc);

    layout->addWidget(formulaFrame);
    layout->addStretch();
}

void OverviewWidget::applyTheme()
{
    // Update main widget background
    setStyleSheet(QString("background-color: %1;").arg(g_theme.windowBg));

    // Update the title
    auto *title = findChild<QLabel*>();
    if (title) title->setStyleSheet(QString("font-size: 16px; font-weight: bold; color: %1;").arg(g_theme.titleText));

    // Update canvas directly
    if (m_canvas) {
        m_canvas->setStyleSheet(QString("background-color: %1; border: 1px solid %2; border-radius: 8px;").arg(g_theme.baseBg).arg(g_theme.border));
    }

    // Rebuild content cards and formula
    rebuildContent();
}

void OverviewWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    // Architecture drawing is handled via canvas widget's paintEvent
}
