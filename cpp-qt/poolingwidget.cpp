#include "poolingwidget.h"
#include "theme.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QRandomGenerator>
#include <cmath>
#include <algorithm>

PoolingWidget::PoolingWidget(QWidget *parent)
    : QWidget(parent), m_animTimer(new QTimer(this))
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(10);

    m_title = new QLabel(QString::fromUtf8("🎮 池化运算交互演示"));
    m_title->setStyleSheet(g_theme.titleStyle());
    layout->addWidget(m_title);

    m_desc = new QLabel(QString::fromUtf8("点击单元格可自定义数值，选择池化类型观察计算过程。"));
    m_desc->setStyleSheet(g_theme.descStyle());
    layout->addWidget(m_desc);

    // Controls
    m_ctrlFrame = new QFrame(this);
    m_ctrlFrame->setStyleSheet(g_theme.panelStyle());
    auto *ctrlLayout = new QHBoxLayout(m_ctrlFrame);
    ctrlLayout->setContentsMargins(10, 8, 10, 8);

    auto addCombo = [&](const QString &label, QComboBox *&combo, const QStringList &items, bool triggerOnChange) {
        auto *f = new QWidget(m_ctrlFrame);
        auto *fl = new QHBoxLayout(f);
        fl->setContentsMargins(0, 0, 0, 0);
        auto *lbl = new QLabel(label, f);
        lbl->setStyleSheet(g_theme.ctrlLabelStyle());
        fl->addWidget(lbl);
        combo = new QComboBox(f);
        combo->addItems(items);
        combo->setStyleSheet(g_theme.comboStyle());
        if (triggerOnChange)
            QObject::connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PoolingWidget::onParamChanged);
        fl->addWidget(combo);
        ctrlLayout->addWidget(f);
    };

    addCombo(QString::fromUtf8("输入大小:"), m_inputSizeCombo, {"3", "4", "5", "6", "8"}, true);
    addCombo(QString::fromUtf8("池化类型:"), m_poolTypeCombo, {"max", "avg"}, true);
    addCombo(QString::fromUtf8("窗口大小:"), m_poolSizeCombo, {"2", "3"}, true);
    addCombo(QString::fromUtf8("步幅:"), m_strideCombo, {"2", "1"}, false);

    m_runBtn = new QPushButton(QString::fromUtf8("▶ 执行池化"), m_ctrlFrame);
    m_runBtn->setStyleSheet(g_theme.btnPrimStyle());
    connect(m_runBtn, &QPushButton::clicked, this, &PoolingWidget::runPooling);
    ctrlLayout->addWidget(m_runBtn);

    m_resetBtn = new QPushButton(QString::fromUtf8("↻ 重置"), m_ctrlFrame);
    m_resetBtn->setStyleSheet(g_theme.btnSecStyle());
    connect(m_resetBtn, &QPushButton::clicked, this, &PoolingWidget::resetDemo);
    ctrlLayout->addWidget(m_resetBtn);

    m_stepInfo = new QLabel("", m_ctrlFrame);
    m_stepInfo->setStyleSheet(QString("color: %1; font-size: 11px;").arg(g_theme.isDark ? "#ec4899" : "#be185d"));
    ctrlLayout->addWidget(m_stepInfo);
    ctrlLayout->addStretch();

    layout->addWidget(m_ctrlFrame);

    // Matrix area (scrollable, left-aligned like convolution)
    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    scrollArea->viewport()->installEventFilter(this);

    auto *matrixArea = new QWidget(scrollArea);
    auto *matrixLayout = new QHBoxLayout(matrixArea);
    matrixLayout->setSpacing(20);
    matrixLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    m_inputMatrix = new MatrixGrid(4, 4, true, false, this);
    m_outputMatrix = new MatrixGrid(1, 1, false, true, this);

    // Input group with size label
    auto *inputGroup = new QWidget(matrixArea);
    auto *inputGroupLayout = new QVBoxLayout(inputGroup);
    inputGroupLayout->setSpacing(4);

    m_inputLabel = new QLabel("", inputGroup);
    m_inputLabel->setStyleSheet(g_theme.labelStyle());
    inputGroupLayout->addWidget(m_inputLabel);
    inputGroupLayout->addWidget(m_inputMatrix);

    auto *outLabel = new QLabel(QString::fromUtf8("输出特征图"), matrixArea);
    m_outputLabel = outLabel;
    outLabel->setStyleSheet(g_theme.labelStyle());

    auto *arrowLabel = new QLabel(QString::fromUtf8("→"), matrixArea);
    arrowLabel->setStyleSheet(QString("font-size: 28px; color: %1;").arg(g_theme.accent));

    matrixLayout->addWidget(inputGroup);
    matrixLayout->addWidget(arrowLabel);

    auto *outGroup = new QWidget(matrixArea);
    auto *outGroupLayout = new QVBoxLayout(outGroup);
    outGroupLayout->setSpacing(4);
    outGroupLayout->addWidget(outLabel);
    outGroupLayout->addWidget(m_outputMatrix);
    matrixLayout->addWidget(outGroup);

    scrollArea->setWidget(matrixArea);
    layout->addWidget(scrollArea);

    connect(m_animTimer, &QTimer::timeout, this, &PoolingWidget::animationStep);
    m_inputSizeCombo->setCurrentText("4");
    m_poolTypeCombo->setCurrentText("max");
    m_poolSizeCombo->setCurrentText("2");
    m_strideCombo->setCurrentText("2");
    initMatrices();
}

void PoolingWidget::onParamChanged()
{
    clearAnimation();
    initMatrices();
}

void PoolingWidget::initMatrices()
{
    int inputSize = m_inputSizeCombo->currentText().toInt();
    int ps = m_poolSizeCombo->currentText().toInt();
    int st = m_strideCombo->currentText().toInt();
    int outSize = (inputSize - ps) / st + 1;

    if (outSize < 1) {
        outSize = 1;
    }

    QVector<QVector<int>> inputData(inputSize, QVector<int>(inputSize));
    for (int i = 0; i < inputSize; ++i)
        for (int j = 0; j < inputSize; ++j)
            inputData[i][j] = QRandomGenerator::global()->bounded(-9, 10);

    m_inputLabel->setText(QString::fromUtf8("输入矩阵 (%1×%2)").arg(inputSize).arg(inputSize));
    m_inputMatrix->setZoomFactor(m_zoomFactor);
    m_inputMatrix->rebuild(inputSize, inputSize, &inputData);
    m_outputMatrix->setZoomFactor(m_zoomFactor);
    m_outputMatrix->rebuild(outSize, outSize);
    for (int i = 0; i < outSize; ++i)
        for (int j = 0; j < outSize; ++j)
            m_outputMatrix->setOutputCell(i, j, "—", 0);

    m_stepInfo->setText("");
}

void PoolingWidget::clearAnimation()
{
    m_animTimer->stop();
}

// ── Zoom & event filter ───────────────────────────────

bool PoolingWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Wheel) {
        auto *we = static_cast<QWheelEvent *>(event);
        if (we->modifiers() & Qt::ControlModifier) {
            double old = m_zoomFactor;
            m_zoomFactor += we->angleDelta().y() > 0 ? 0.1 : -0.1;
            m_zoomFactor = std::clamp(m_zoomFactor, 0.4, 3.0);
            if (!qFuzzyCompare(m_zoomFactor, old)) {
                applyZoom();
            }
            we->accept();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void PoolingWidget::applyZoom()
{
    clearAnimation();

    int inputSize = m_inputSizeCombo->currentText().toInt();
    int ps = m_poolSizeCombo->currentText().toInt();
    int st = m_strideCombo->currentText().toInt();
    int outSize = (inputSize - ps) / st + 1;
    if (outSize < 1) outSize = 1;

    m_inputMatrix->setZoomFactor(m_zoomFactor);
    m_inputMatrix->rebuild(inputSize, inputSize);
    m_outputMatrix->setZoomFactor(m_zoomFactor);
    m_outputMatrix->rebuild(outSize, outSize);

    // Update label font sizes
    int lfs = std::max(7, int(10 * m_zoomFactor));
    QString labelStyle = QString("font-size: %1px; font-weight: bold; color: %2;").arg(lfs).arg(g_theme.mutedText);
    m_inputLabel->setStyleSheet(labelStyle);
    m_outputLabel->setStyleSheet(labelStyle);
}

void PoolingWidget::applyTheme()
{
    clearAnimation();

    // Update title & description
    m_title->setStyleSheet(g_theme.titleStyle());
    m_desc->setStyleSheet(g_theme.descStyle());

    // Update control panel
    m_ctrlFrame->setStyleSheet(g_theme.panelStyle());
    m_runBtn->setStyleSheet(g_theme.btnPrimStyle());
    m_resetBtn->setStyleSheet(g_theme.btnSecStyle());
    m_stepInfo->setStyleSheet(QString("color: %1; font-size: 11px;").arg(g_theme.stepInfoColor));

    // Update combo box and label styles inside control panel
    const auto combos = m_ctrlFrame->findChildren<QComboBox*>();
    for (auto *combo : combos)
        combo->setStyleSheet(g_theme.comboStyle());
    const auto labels = m_ctrlFrame->findChildren<QLabel*>();
    for (auto *lbl : labels) {
        if (lbl != m_stepInfo)
            lbl->setStyleSheet(g_theme.ctrlLabelStyle());
    }

    m_inputMatrix->applyTheme();
    m_outputMatrix->applyTheme();

    int lfs = std::max(7, int(10 * m_zoomFactor));
    QString ls = QString("font-size: %1px; font-weight: bold; color: %2;").arg(lfs).arg(g_theme.mutedText);
    m_inputLabel->setStyleSheet(ls);
    m_outputLabel->setStyleSheet(ls);
}

void PoolingWidget::resetDemo()
{
    clearAnimation();
    initMatrices();
}

void PoolingWidget::runPooling()
{
    clearAnimation();

    m_isMaxPool = (m_poolTypeCombo->currentText() == "max");
    int ps = m_poolSizeCombo->currentText().toInt();
    int st = m_strideCombo->currentText().toInt();
    m_animInputSize = m_inputSizeCombo->currentText().toInt();
    m_animOutSize = (m_animInputSize - ps) / st + 1;
    if (m_animOutSize < 1) m_animOutSize = 1;

    // ── Sync latest values from the on-screen widgets ──
    m_inputMatrix->syncFromWidgets();

    auto inputData = m_inputMatrix->data();

    // Compute output
    m_animOutput.clear();
    m_animOutput.resize(m_animOutSize, QVector<double>(m_animOutSize));
    for (int i = 0; i < m_animOutSize; ++i) {
        for (int j = 0; j < m_animOutSize; ++j) {
            QVector<int> vals;
            for (int m = 0; m < ps; ++m) {
                for (int n = 0; n < ps; ++n) {
                    int ri = i * st + m;
                    int rj = j * st + n;
                    if (ri < m_animInputSize && rj < m_animInputSize)
                        vals.append(inputData[ri][rj]);
                }
            }
            if (m_isMaxPool) {
                m_animOutput[i][j] = *std::max_element(vals.begin(), vals.end());
            } else {
                double sum = 0;
                for (int v : vals) sum += v;
                m_animOutput[i][j] = std::round(sum / vals.size() * 10.0) / 10.0;
            }
        }
    }

    m_outputMatrix->rebuild(m_animOutSize, m_animOutSize);

    m_animStep = 0;
    m_animTotal = m_animOutSize * m_animOutSize;
    m_animPoolSize = ps;
    m_animStride = st;

    m_animTimer->start(450);
}

void PoolingWidget::animationStep()
{
    if (m_animStep >= m_animTotal) {
        m_animTimer->stop();
        m_stepInfo->setText(QString::fromUtf8("%1 Pooling 完成！")
                           .arg(m_isMaxPool ? "Max" : "Average"));
        return;
    }

    int oi = m_animStep / m_animOutSize;
    int oj = m_animStep % m_animOutSize;
    double val = m_animOutput[oi][oj];

    m_outputMatrix->setOutputCell(oi, oj, QString::number(val, 'f', val == int(val) ? 0 : 1), static_cast<int>(val));

    // Highlight input region
    m_inputMatrix->clearHighlights();
    auto inputData = m_inputMatrix->data();
    double maxVal = -1e9;
    int maxR = -1, maxC = -1;
    for (int m = 0; m < m_animPoolSize; ++m) {
        for (int n = 0; n < m_animPoolSize; ++n) {
            int ir = oi * m_animStride + m;
            int ic = oj * m_animStride + n;
            if (ir < inputData.size() && ic < inputData[ir].size()) {
                m_inputMatrix->highlightCell(ir, ic, QColor("#ec4899"));
                if (m_isMaxPool && inputData[ir][ic] > maxVal) {
                    maxVal = inputData[ir][ic];
                    maxR = ir;
                    maxC = ic;
                }
            }
        }
    }
    if (m_isMaxPool && maxR >= 0) {
        m_inputMatrix->highlightCell(maxR, maxC, QColor("#f59e0b"));
    }

    QString label = m_isMaxPool ? QString::fromUtf8("最大值") : QString::fromUtf8("平均值");
    m_stepInfo->setText(QString::fromUtf8("窗口 (%1,%2): %3 = %4")
                       .arg(oi).arg(oj).arg(label).arg(val, 0, 'f', val == int(val) ? 0 : 1));

    m_animStep++;
}
