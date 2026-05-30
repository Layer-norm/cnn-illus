#include "convolutionwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QRandomGenerator>
#include <QScrollArea>
#include <cmath>

// ── MatrixGrid ────────────────────────────────────

int MatrixGrid::cellSize(int matrixSize)
{
    if (matrixSize <= 5) return 40;
    if (matrixSize <= 7) return 36;
    if (matrixSize <= 9) return 30;
    if (matrixSize <= 11) return 26;
    return 22;
}

QColor MatrixGrid::valueColor(int value, int lo, int hi) const
{
    double t = std::clamp(double(value - lo) / (hi - lo), 0.0, 1.0);
    double hue = 220.0 - t * 170.0;
    return QColor::fromHslF(hue / 360.0, 0.6, 0.35);
}

MatrixGrid::MatrixGrid(int rows, int cols, bool editable, bool readOnly, QWidget *parent)
    : QWidget(parent), m_rows(rows), m_cols(cols), m_editable(editable), m_readOnly(readOnly)
{
    m_grid = new QGridLayout(this);
    m_grid->setSpacing(2);
    m_grid->setContentsMargins(0, 0, 0, 0);
    buildGrid();
}

void MatrixGrid::buildGrid(const QVector<QVector<int>> *data)
{
    int cs = int(cellSize(std::max(m_rows, m_cols)) * m_zoomFactor);
    int fontSize = std::max(7, int(11 * m_zoomFactor));
    m_cells.clear();
    m_entries.clear();
    m_labels.clear();

    if (data) {
        m_data = *data;
    } else if (m_data.isEmpty()) {
        m_data.resize(m_rows);
        for (int i = 0; i < m_rows; ++i) {
            m_data[i].resize(m_cols);
            for (int j = 0; j < m_cols; ++j) {
                m_data[i][j] = QRandomGenerator::global()->bounded(-9, 10);
            }
        }
    }

    m_cells.resize(m_rows);
    m_entries.resize(m_rows);
    m_labels.resize(m_rows);

    for (int i = 0; i < m_rows; ++i) {
        m_cells[i].resize(m_cols);
        m_entries[i].resize(m_cols, nullptr);
        m_labels[i].resize(m_cols, nullptr);
        for (int j = 0; j < m_cols; ++j) {
            int v = (i < m_data.size() && j < m_data[i].size()) ? m_data[i][j] : 0;
            auto *cell = new QWidget(this);
            cell->setFixedSize(cs, cs);
            cell->setStyleSheet(QString("background-color: %1; border: 1px solid #232b42; border-radius: 3px;")
                                .arg(valueColor(v).name()));
            cell->setProperty("row", i);
            cell->setProperty("col", j);

            if (m_readOnly) {
                auto *lbl = new QLabel(QString::number(v), cell);
                lbl->setAlignment(Qt::AlignCenter);
                lbl->setStyleSheet(QString("color: %1; font-weight: bold; font-size: %2px; background: transparent;")
                                   .arg(valueColor(v).lighter(150).name()).arg(fontSize));
                lbl->setGeometry(0, 0, cs, cs);
                m_labels[i][j] = lbl;
            } else if (m_editable) {
                auto *entry = new QLineEdit(cell);
                entry->setText(QString::number(v));
                entry->setAlignment(Qt::AlignCenter);
                entry->setStyleSheet(QString(
                    "QLineEdit { background: transparent; color: %1; font-weight: bold; font-size: %2px; border: none; }"
                ).arg(valueColor(v).lighter(150).name()).arg(fontSize));
                entry->setGeometry(2, 2, cs - 4, cs - 4);
                connect(entry, &QLineEdit::returnPressed, this, [this, entry, i, j, cell, fontSize]() {
                    bool ok;
                    int val = entry->text().toInt(&ok);
                    if (!ok) val = 0;
                    if (i < m_data.size() && j < m_data[i].size()) {
                        m_data[i][j] = val;
                    }
                    entry->setStyleSheet(QString(
                        "QLineEdit { background: transparent; color: %1; font-weight: bold; font-size: %2px; border: none; }"
                    ).arg(valueColor(val).lighter(150).name()).arg(fontSize));
                    cell->setStyleSheet(QString("background-color: %1; border: 1px solid #232b42; border-radius: 3px;")
                                        .arg(valueColor(val).name()));
                    emit cellChanged(i, j, val);
                });
                m_entries[i][j] = entry;
            } else {
                auto *lbl = new QLabel(QString::number(v), cell);
                lbl->setAlignment(Qt::AlignCenter);
                lbl->setStyleSheet(QString("color: %1; font-weight: bold; font-size: %2px; background: transparent;")
                                   .arg(valueColor(v).lighter(150).name()).arg(fontSize));
                lbl->setGeometry(0, 0, cs, cs);
                m_labels[i][j] = lbl;
            }
            m_grid->addWidget(cell, i, j);
        }
    }
}

void MatrixGrid::setZoomFactor(double factor)
{
    m_zoomFactor = factor;
}

void MatrixGrid::syncFromWidgets()
{
    for (int i = 0; i < m_entries.size() && i < m_data.size(); ++i) {
        for (int j = 0; j < m_entries[i].size() && j < m_data[i].size(); ++j) {
            auto *entry = m_entries[i][j];
            if (!entry) continue;
            bool ok;
            int val = entry->text().toInt(&ok);
            if (ok) m_data[i][j] = val;
        }
    }
}

void MatrixGrid::setData(const QVector<QVector<int>> &data)
{
    m_data = data;
    m_rows = data.size();
    m_cols = data.isEmpty() ? 0 : data[0].size();
    rebuild(m_rows, m_cols, &data);
}

QVector<QVector<int>> MatrixGrid::data() const
{
    return m_data;
}

void MatrixGrid::setCellValue(int r, int c, int value)
{
    if (r >= 0 && r < m_data.size() && c >= 0 && c < m_data[r].size()) {
        m_data[r][c] = value;
    }
}

int MatrixGrid::cellValue(int r, int c) const
{
    if (r >= 0 && r < m_data.size() && c >= 0 && c < m_data[r].size()) {
        return m_data[r][c];
    }
    return 0;
}

void MatrixGrid::highlightCell(int r, int c, const QColor &color)
{
    auto *item = m_grid->itemAtPosition(r, c);
    if (item && item->widget()) {
        item->widget()->setStyleSheet(
            QString("background-color: %1; border: 2px solid %2; border-radius: 3px;")
            .arg(valueColor(m_data[r][c]).name()).arg(color.name()));
    }
}

void MatrixGrid::clearHighlights()
{
    for (int i = 0; i < m_rows; ++i) {
        for (int j = 0; j < m_cols; ++j) {
            auto *item = m_grid->itemAtPosition(i, j);
            if (item && item->widget()) {
                item->widget()->setStyleSheet(
                    QString("background-color: %1; border: 1px solid #232b42; border-radius: 3px;")
                    .arg(valueColor(m_data[i][j]).name()));
            }
        }
    }
}

void MatrixGrid::setOutputCell(int r, int c, const QString &text, int value)
{
    if (r < 0 || r >= m_rows || c < 0 || c >= m_cols) return;
    auto *item = m_grid->itemAtPosition(r, c);
    if (!item || !item->widget()) return;

    auto *w = item->widget();
    auto *lbl = w->findChild<QLabel*>();
    if (lbl) {
        lbl->setText(text);
        int fs = std::max(7, int(11 * m_zoomFactor));
        lbl->setStyleSheet(QString("color: %1; font-weight: bold; font-size: %2px; background: transparent;")
                          .arg(QColor::fromHslF(std::clamp(220.0 - std::clamp(double(value + 50) / 100, 0.0, 1.0) * 170.0, 0.0, 360.0) / 360.0, 0.7, 0.65).name()).arg(fs));
    }
    w->setStyleSheet(QString("background-color: %1; border: 1px solid #232b42; border-radius: 3px;")
                    .arg(QColor::fromHslF(std::clamp(220.0 - std::clamp(double(value + 50) / 100, 0.0, 1.0) * 170.0, 0.0, 360.0) / 360.0, 0.6, 0.35).name()));
}

void MatrixGrid::rebuild(int rows, int cols, const QVector<QVector<int>> *data)
{
    // Clear existing
    QLayoutItem *item;
    while ((item = m_grid->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    m_rows = rows;
    m_cols = cols;
    m_cells.clear();
    m_entries.clear();
    m_labels.clear();
    if (data) {
        m_data = *data;
    } else if (m_data.size() != rows || (rows > 0 && m_data[0].size() != cols)) {
        m_data.resize(rows);
        for (int i = 0; i < rows; ++i) {
            m_data[i].resize(cols);
            for (int j = 0; j < cols; ++j) {
                m_data[i][j] = QRandomGenerator::global()->bounded(-9, 10);
            }
        }
    }
    buildGrid();
}

// ── Color helpers for ConvolutionWidget ────────────

QString ConvolutionWidget::cellBgColor(int val, int lo, int hi)
{
    double t = std::clamp(double(val - lo) / (hi - lo), 0.0, 1.0);
    double hue = 220.0 - t * 170.0;
    return QColor::fromHslF(hue / 360.0, 0.6, 0.35).name();
}

QString ConvolutionWidget::cellFgColor(int val, int lo, int hi)
{
    double t = std::clamp(double(val - lo) / (hi - lo), 0.0, 1.0);
    double hue = 220.0 - t * 170.0;
    return QColor::fromHslF(hue / 360.0, 0.7, 0.65).name();
}

// ── ConvolutionWidget ──────────────────────────────

ConvolutionWidget::ConvolutionWidget(QWidget *parent)
    : QWidget(parent), m_animTimer(new QTimer(this)), m_inputGrid(nullptr), m_inputGroup(nullptr), m_currentPad(0)
{
    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(15, 15, 15, 15);
    outerLayout->setSpacing(10);

    // Title
    auto *title = new QLabel(QString::fromUtf8("🎮 卷积运算交互演示"));
    title->setStyleSheet("font-size: 15px; font-weight: bold; color: #e2e8f0;");
    outerLayout->addWidget(title);

    auto *desc = new QLabel(QString::fromUtf8("点击单元格可自定义数值，点击「执行卷积」观察滑动计算过程。"));
    desc->setStyleSheet("font-size: 11px; color: #64748b;");
    outerLayout->addWidget(desc);

    // Controls
    auto *ctrlFrame = new QFrame(this);
    ctrlFrame->setStyleSheet("QFrame { background-color: #1a2035; border: 1px solid #232b42; border-radius: 8px; }");
    auto *ctrlLayout = new QHBoxLayout(ctrlFrame);
    ctrlLayout->setContentsMargins(10, 8, 10, 8);

    auto addCombo = [&](const QString &label, QComboBox *&combo, const QStringList &items, bool triggerOnChange) {
        auto *f = new QWidget(ctrlFrame);
        auto *fl = new QHBoxLayout(f);
        fl->setContentsMargins(0, 0, 0, 0);
        auto *lbl = new QLabel(label, f);
        lbl->setStyleSheet("color: #94a3b8; font-size: 11px;");
        fl->addWidget(lbl);
        combo = new QComboBox(f);
        combo->addItems(items);
        combo->setStyleSheet(
            "QComboBox { background: #232b42; color: #e2e8f0; border: 1px solid #1a2035; "
            "border-radius: 4px; padding: 3px 6px; font-size: 11px; }"
            "QComboBox::drop-down { border: none; }"
            "QComboBox QAbstractItemView { background: #232b42; color: #e2e8f0; selection-background-color: #3b82f6; }"
        );
        if (triggerOnChange)
            QObject::connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ConvolutionWidget::onParamChanged);
        fl->addWidget(combo);
        ctrlLayout->addWidget(f);
    };

    addCombo(QString::fromUtf8("输入大小:"), m_inputSizeCombo, {"3", "4", "5", "6", "7"}, true);
    addCombo(QString::fromUtf8("核大小:"), m_kernelSizeCombo, {"1", "3", "5"}, true);
    addCombo(QString::fromUtf8("步幅:"), m_strideCombo, {"1", "2"}, false);
    addCombo(QString::fromUtf8("填充:"), m_paddingCombo, {"0", "1", "2"}, true);

    m_runBtn = new QPushButton(QString::fromUtf8("▶ 执行卷积"), ctrlFrame);
    m_runBtn->setStyleSheet(
        "QPushButton { background-color: #3b82f6; color: white; border: none; "
        "border-radius: 4px; padding: 6px 14px; font-weight: bold; font-size: 11px; }"
        "QPushButton:hover { background-color: #6366f1; }"
    );
    connect(m_runBtn, &QPushButton::clicked, this, &ConvolutionWidget::runConvolution);
    ctrlLayout->addWidget(m_runBtn);

    m_resetBtn = new QPushButton(QString::fromUtf8("↻ 重置"), ctrlFrame);
    m_resetBtn->setStyleSheet(
        "QPushButton { background-color: #232b42; color: #e2e8f0; border: none; "
        "border-radius: 4px; padding: 6px 14px; font-size: 11px; }"
        "QPushButton:hover { background-color: #1a2035; }"
    );
    connect(m_resetBtn, &QPushButton::clicked, this, &ConvolutionWidget::resetDemo);
    ctrlLayout->addWidget(m_resetBtn);

    m_stepInfo = new QLabel("", ctrlFrame);
    m_stepInfo->setStyleSheet("color: #06b6d4; font-size: 11px;");
    ctrlLayout->addWidget(m_stepInfo);
    ctrlLayout->addStretch();

    outerLayout->addWidget(ctrlFrame);

    // Matrix area (scrollable)
    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    scrollArea->viewport()->installEventFilter(this);

    auto *matrixArea = new QWidget(scrollArea);
    m_matrixAreaLayout = new QHBoxLayout(matrixArea);
    m_matrixAreaLayout->setSpacing(20);
    m_matrixAreaLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    scrollArea->setWidget(matrixArea);
    outerLayout->addWidget(scrollArea);

    // Input group (custom padded grid built later)
    m_inputGroup = new QWidget(matrixArea);
    m_inputGroupLayout = new QVBoxLayout(m_inputGroup);
    m_inputGroupLayout->setSpacing(4);

    m_inputLabel = new QLabel("", m_inputGroup);
    m_inputLabel->setStyleSheet("font-size: 10px; font-weight: bold; color: #64748b;");
    m_inputGroupLayout->addWidget(m_inputLabel);

    m_inputGridContainer = new QWidget(m_inputGroup);
    m_inputGroupLayout->addWidget(m_inputGridContainer);

    // Kernel group
    m_kernelMatrix = new MatrixGrid(3, 3, true, false, this);
    m_kernelLabel = new QLabel(QString::fromUtf8("卷积核 (3×3)"), this);
    m_kernelLabel->setStyleSheet("font-size: 10px; font-weight: bold; color: #64748b;");

    auto *kernelGroup = new QWidget(matrixArea);
    auto *kernelGroupLayout = new QVBoxLayout(kernelGroup);
    kernelGroupLayout->setSpacing(4);
    kernelGroupLayout->addWidget(m_kernelLabel);
    kernelGroupLayout->addWidget(m_kernelMatrix);

    auto *arrowLabel = new QLabel(QString::fromUtf8("→"), matrixArea);
    arrowLabel->setStyleSheet("font-size: 28px; color: #3b82f6;");

    // Output group
    m_outputMatrix = new MatrixGrid(1, 1, false, true, this);

    auto *outputGroup = new QWidget(matrixArea);
    auto *outputGroupLayout = new QVBoxLayout(outputGroup);
    outputGroupLayout->setSpacing(4);
    m_outputLabel = new QLabel(QString::fromUtf8("输出特征图"), this);
    m_outputLabel->setStyleSheet("font-size: 10px; font-weight: bold; color: #64748b;");
    outputGroupLayout->addWidget(m_outputLabel);
    outputGroupLayout->addWidget(m_outputMatrix);

    m_matrixAreaLayout->addWidget(m_inputGroup);
    m_matrixAreaLayout->addWidget(kernelGroup);
    m_matrixAreaLayout->addWidget(arrowLabel);
    m_matrixAreaLayout->addWidget(outputGroup);

    connect(m_animTimer, &QTimer::timeout, this, &ConvolutionWidget::animationStep);
    m_inputSizeCombo->setCurrentText("5");
    m_kernelSizeCombo->setCurrentText("3");
    initMatrices();
}

void ConvolutionWidget::onParamChanged()
{
    clearAnimation();
    initMatrices();
}

void ConvolutionWidget::rebuildInputGrid(int paddedSize, int inputSize, int pad, int cellSz)
{
    m_currentPad = pad;

    // Clear old grid inside container
    if (m_inputGrid) {
        QLayoutItem *child;
        while ((child = m_inputGrid->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }
        delete m_inputGrid;
    }

    m_inputGrid = new QGridLayout(m_inputGridContainer);
    m_inputGrid->setSpacing(2);
    m_inputGrid->setContentsMargins(0, 0, 0, 0);

    m_inputCells.clear();
    m_inputCells.resize(paddedSize, QVector<QWidget*>(paddedSize, nullptr));

    for (int i = 0; i < paddedSize; ++i) {
        for (int j = 0; j < paddedSize; ++j) {
            bool isPad = (i < pad || i >= pad + inputSize || j < pad || j >= pad + inputSize);

            auto *cell = new QWidget(m_inputGridContainer);
            cell->setFixedSize(cellSz, cellSz);

            if (isPad) {
                // Padding cell: gray, show "0", non-editable
                cell->setStyleSheet(
                    QString("background-color: %1; border: 1px dashed #64748b; border-radius: 3px;")
                    .arg("#1a2035"));
                auto *lbl = new QLabel("0", cell);
                lbl->setAlignment(Qt::AlignCenter);
                lbl->setGeometry(0, 0, cellSz, cellSz);

                int fs = std::max(7, int(11 * cellSz / MatrixGrid::cellSize(std::max(paddedSize, 3))));
                lbl->setStyleSheet(QString("color: #64748b; font-weight: bold; font-size: %1px; background: transparent;").arg(fs));
            } else {
                // Content cell: editable
                int ri = i - pad;
                int rj = j - pad;
                int v = (ri < m_inputData.size() && rj < m_inputData[ri].size()) ? m_inputData[ri][rj] : 0;
                QString bg = cellBgColor(v);
                QString fg = cellFgColor(v);

                cell->setStyleSheet(QString("background-color: %1; border: 1px solid #232b42; border-radius: 3px;").arg(bg));

                auto *entry = new QLineEdit(cell);
                entry->setText(QString::number(v));
                entry->setAlignment(Qt::AlignCenter);
                entry->setStyleSheet(QString(
                    "QLineEdit { background: transparent; color: %1; font-weight: bold; font-size: %2px; border: none; }"
                ).arg(fg).arg(std::max(7, int(11 * cellSz / MatrixGrid::cellSize(std::max(paddedSize, 3))))));
                entry->setGeometry(2, 2, cellSz - 4, cellSz - 4);

                int eRi = ri, eRj = rj;
                connect(entry, &QLineEdit::returnPressed, this, [this, entry, eRi, eRj, pad, cellSz]() {
                    bool ok;
                    int val = entry->text().toInt(&ok);
                    if (!ok) val = 0;
                    if (eRi < m_inputData.size() && eRj < m_inputData[eRi].size()) {
                        m_inputData[eRi][eRj] = val;
                    }
                    // Update cell colors in the padded grid
                    rebuildInputGrid(m_inputData.size() + 2 * m_currentPad, m_inputData.size(), m_currentPad, cellSz);
                });
            }

            m_inputCells[i][j] = cell;
            m_inputGrid->addWidget(cell, i, j);
        }
    }
}

void ConvolutionWidget::initMatrices()
{
    int inputSize = m_inputSizeCombo->currentText().toInt();
    int ks = m_kernelSizeCombo->currentText().toInt();
    int pad = m_paddingCombo->currentText().toInt();
    int stride = m_strideCombo->currentText().toInt();

    int paddedSize = inputSize + 2 * pad;
    int outSize = (paddedSize - ks) / stride + 1;
    int cellSz = int(MatrixGrid::cellSize(std::max(paddedSize, ks)) * m_zoomFactor);

    m_inputLabel->setText(QString::fromUtf8("输入矩阵 (%1×%2%3)")
                          .arg(paddedSize).arg(paddedSize)
                          .arg(pad > 0 ? QString::fromUtf8(", 灰色为 padding") : ""));

    // Generate random input data (inputSize x inputSize, WITHOUT padding)
    m_inputData.clear();
    m_inputData.resize(inputSize, QVector<int>(inputSize));
    for (int i = 0; i < inputSize; ++i)
        for (int j = 0; j < inputSize; ++j)
            m_inputData[i][j] = QRandomGenerator::global()->bounded(-9, 10);

    // Build the padded grid display
    rebuildInputGrid(paddedSize, inputSize, pad, cellSz);

    // Kernel matrix
    QVector<QVector<int>> kernelData(ks, QVector<int>(ks));
    for (int i = 0; i < ks; ++i)
        for (int j = 0; j < ks; ++j)
            kernelData[i][j] = QRandomGenerator::global()->bounded(-9, 10);

    m_kernelLabel->setText(QString::fromUtf8("卷积核 (%1×%2)").arg(ks).arg(ks));
    m_kernelMatrix->rebuild(ks, ks, &kernelData);

    // Output matrix
    m_outputMatrix->rebuild(outSize, outSize);
    for (int i = 0; i < outSize; ++i)
        for (int j = 0; j < outSize; ++j)
            m_outputMatrix->setOutputCell(i, j, "—", 0);

    m_stepInfo->setText("");
}

void ConvolutionWidget::clearAnimation()
{
    m_animTimer->stop();
}

// ── Zoom & event filter ───────────────────────────────

bool ConvolutionWidget::eventFilter(QObject *obj, QEvent *event)
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

void ConvolutionWidget::applyZoom()
{
    clearAnimation();

    int inputSize = m_inputSizeCombo->currentText().toInt();
    int ks = m_kernelSizeCombo->currentText().toInt();
    int pad = m_paddingCombo->currentText().toInt();
    int stride = m_strideCombo->currentText().toInt();
    int paddedSize = inputSize + 2 * pad;
    int outSize = (paddedSize - ks) / stride + 1;
    int cellSz = int(MatrixGrid::cellSize(std::max(paddedSize, ks)) * m_zoomFactor);

    // Apply zoom to MatrixGrid instances and rebuild
    m_kernelMatrix->setZoomFactor(m_zoomFactor);
    m_kernelMatrix->rebuild(ks, ks);
    m_outputMatrix->setZoomFactor(m_zoomFactor);
    m_outputMatrix->rebuild(outSize, outSize);

    rebuildInputGrid(paddedSize, inputSize, pad, cellSz);

    // Update label font sizes
    int lfs = std::max(7, int(10 * m_zoomFactor));
    QString labelStyle = QString("font-size: %1px; font-weight: bold; color: #64748b;").arg(lfs);
    m_inputLabel->setStyleSheet(labelStyle);
    m_kernelLabel->setStyleSheet(labelStyle);
    m_outputLabel->setStyleSheet(labelStyle);
}

void ConvolutionWidget::resetDemo()
{
    clearAnimation();
    initMatrices();
}

void ConvolutionWidget::runConvolution()
{
    clearAnimation();

    int inputSize = m_inputSizeCombo->currentText().toInt();
    int ks = m_kernelSizeCombo->currentText().toInt();
    int stride = m_strideCombo->currentText().toInt();
    int pad = m_paddingCombo->currentText().toInt();
    int paddedSize = inputSize + 2 * pad;
    int outSize = (paddedSize - ks) / stride + 1;

    // ── Sync latest values from the on-screen widgets ──
    // Input grid: read from QLineEdit widgets in the padded grid
    for (int i = 0; i < paddedSize && i < m_inputCells.size(); ++i) {
        for (int j = 0; j < paddedSize && j < m_inputCells[i].size(); ++j) {
            bool isPad = (i < pad || i >= pad + inputSize || j < pad || j >= pad + inputSize);
            if (isPad) continue;
            QWidget *cell = m_inputCells[i][j];
            if (!cell) continue;
            auto *entry = cell->findChild<QLineEdit*>();
            if (!entry) continue;
            int ri = i - pad, rj = j - pad;
            if (ri >= m_inputData.size() || rj >= m_inputData[ri].size()) continue;
            bool ok;
            int val = entry->text().toInt(&ok);
            if (ok) m_inputData[ri][rj] = val;
        }
    }

    // Kernel matrix: sync from MatrixGrid's QLineEdit entries
    m_kernelMatrix->syncFromWidgets();

    auto kernelData = m_kernelMatrix->data();

    // Build padded matrix from m_inputData
    QVector<QVector<int>> padded(paddedSize, QVector<int>(paddedSize, 0));
    for (int i = 0; i < inputSize; ++i)
        for (int j = 0; j < inputSize; ++j)
            padded[i + pad][j + pad] = m_inputData[i][j];

    // Compute output
    m_animOutput.clear();
    m_animOutput.resize(outSize, QVector<int>(outSize));
    for (int i = 0; i < outSize; ++i) {
        for (int j = 0; j < outSize; ++j) {
            int sum = 0;
            for (int m = 0; m < ks; ++m)
                for (int n = 0; n < ks; ++n)
                    sum += padded[i * stride + m][j * stride + n] * kernelData[m][n];
            m_animOutput[i][j] = sum;
        }
    }

    // Reset output matrix
    m_outputMatrix->rebuild(outSize, outSize);

    m_animStep = 0;
    m_animTotal = outSize * outSize;
    m_animPaddedSize = paddedSize;
    m_animKs = ks;
    m_animStride = stride;
    m_animOutSize = outSize;
    m_animInputSize = inputSize;

    // Restore all input cells to their normal color before animation starts
    int cellSz = int(MatrixGrid::cellSize(std::max(paddedSize, ks)) * m_zoomFactor);
    rebuildInputGrid(paddedSize, inputSize, pad, cellSz);

    m_animTimer->start(350);
}

void ConvolutionWidget::animationStep()
{
    // ── 1) Sliding window highlight on input matrix ──
    // Restore all input cells to normal, then highlight the kernel window
    int paddedSize = m_animPaddedSize;
    int pad = m_currentPad;

    // First pass: restore every cell to its normal color
    for (int i = 0; i < paddedSize && i < m_inputCells.size(); ++i) {
        for (int j = 0; j < paddedSize && j < m_inputCells[i].size(); ++j) {
            QWidget *cell = m_inputCells[i][j];
            if (!cell) continue;

            bool isPad = (i < pad || i >= pad + m_animInputSize || j < pad || j >= pad + m_animInputSize);
            if (isPad) {
                cell->setStyleSheet(
                    "background-color: #1a2035; border: 1px dashed #64748b; border-radius: 3px;");
            } else {
                int ri = i - pad;
                int rj = j - pad;
                int v = (ri < m_inputData.size() && rj < m_inputData[ri].size()) ? m_inputData[ri][rj] : 0;
                cell->setStyleSheet(QString("background-color: %1; border: 1px solid #232b42; border-radius: 3px;")
                                    .arg(cellBgColor(v)));
            }
        }
    }

    // Second pass: highlight the current kernel window cells in yellow
    if (m_animStep < m_animTotal) {
        int oi = m_animStep / m_animOutSize;
        int oj = m_animStep % m_animOutSize;

        for (int m = 0; m < m_animKs; ++m) {
            for (int n = 0; n < m_animKs; ++n) {
                int ir = oi * m_animStride + m;
                int ic = oj * m_animStride + n;
                if (ir < paddedSize && ic < paddedSize && ir < m_inputCells.size() && ic < m_inputCells[ir].size()) {
                    QWidget *cell = m_inputCells[ir][ic];
                    if (cell) {
                        cell->setStyleSheet(
                            "background-color: #ffe066; border: 2px solid #f59e0b; border-radius: 3px;");
                    }
                }
            }
        }
    }

    // ── 2) Fill output cell ──
    if (m_animStep >= m_animTotal) {
        m_animTimer->stop();
        m_stepInfo->setText(QString::fromUtf8("卷积完成！输出尺寸: %1×%2").arg(m_animOutSize).arg(m_animOutSize));
        return;
    }

    int oi = m_animStep / m_animOutSize;
    int oj = m_animStep % m_animOutSize;
    int val = m_animOutput[oi][oj];

    m_outputMatrix->setOutputCell(oi, oj, QString::number(val), val);

    m_stepInfo->setText(QString::fromUtf8("位置 (%1,%2): 计算值 = %3").arg(oi).arg(oj).arg(val));
    m_animStep++;
}
