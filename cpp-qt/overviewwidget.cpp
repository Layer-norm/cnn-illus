#include "overviewwidget.h"
#include <QVBoxLayout>
#include <QLabel>

OverviewWidget::OverviewWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    // Title
    auto *title = new QLabel(QString::fromUtf8("🏗️ CNN 网络总体框架"));
    title->setStyleSheet("font-size: 16px; font-weight: bold; color: #e2e8f0;");
    layout->addWidget(title);

    // Architecture canvas
    auto *canvas = new QWidget(this);
    canvas->setMinimumHeight(140);
    canvas->setStyleSheet("background-color: #111827; border: 1px solid #232b42; border-radius: 8px;");
    layout->addWidget(canvas);

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
        auto *frame = new QWidget(this);
        frame->setStyleSheet("background-color: #111827; border: 1px solid #232b42; border-radius: 8px;");
        auto *fl = new QVBoxLayout(frame);
        fl->setContentsMargins(15, 10, 15, 10);

        auto *ct = new QLabel(c.title);
        ct->setStyleSheet(QString("font-size: 13px; font-weight: bold; color: %1;").arg(c.color));
        fl->addWidget(ct);

        auto *cd = new QLabel(c.desc);
        cd->setStyleSheet("font-size: 11px; color: #94a3b8;");
        cd->setWordWrap(true);
        fl->addWidget(cd);

        layout->addWidget(frame);
    }

    // Formula
    auto *formulaFrame = new QWidget(this);
    formulaFrame->setStyleSheet("background-color: #1a2035; border: 1px solid #232b42; border-radius: 8px;");
    auto *formulaLayout = new QVBoxLayout(formulaFrame);
    formulaLayout->setContentsMargins(15, 12, 15, 12);

    auto *formulaTitle = new QLabel(QString::fromUtf8("📐 输出尺寸计算公式"));
    formulaTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #06b6d4;");
    formulaLayout->addWidget(formulaTitle);

    auto *formula = new QLabel(QString::fromUtf8("O = ⌊(I − K + 2P) / S⌋ + 1"));
    formula->setStyleSheet("font-size: 16px; font-weight: bold; color: #06b6d4; padding: 8px 0;");
    formula->setAlignment(Qt::AlignCenter);
    formulaLayout->addWidget(formula);

    auto *desc = new QLabel(QString::fromUtf8("O = 输出尺寸  I = 输入尺寸  K = 卷积核/池化窗口大小  P = 填充大小  S = 步幅"));
    desc->setStyleSheet("font-size: 11px; color: #64748b;");
    desc->setAlignment(Qt::AlignCenter);
    desc->setWordWrap(true);
    formulaLayout->addWidget(desc);

    layout->addWidget(formulaFrame);
    layout->addStretch();
}

void OverviewWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    // Architecture drawing is handled via canvas widget's paintEvent
}
