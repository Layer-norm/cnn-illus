#ifndef CONVOLUTIONWIDGET_H
#define CONVOLUTIONWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QTimer>
#include <QWheelEvent>
#include <QScrollArea>
#include <QFrame>
#include <QVector>

class MatrixGrid : public QWidget
{
    Q_OBJECT
public:
    explicit MatrixGrid(int rows, int cols, bool editable = true, bool readOnly = false, QWidget *parent = nullptr);

    void syncFromWidgets();
    void setData(const QVector<QVector<int>> &data);
    QVector<QVector<int>> data() const;
    void setCellValue(int r, int c, int value);
    int cellValue(int r, int c) const;
    void highlightCell(int r, int c, const QColor &color);
    void clearHighlights();
    void setOutputCell(int r, int c, const QString &text, int value);
    void rebuild(int rows, int cols, const QVector<QVector<int>> *data = nullptr);
    int rows() const { return m_rows; }
    int cols() const { return m_cols; }

    static int cellSize(int matrixSize);
    void setZoomFactor(double factor);
    void applyTheme();

signals:
    void cellChanged(int r, int c, int value);

private:
    void buildGrid(const QVector<QVector<int>> *data = nullptr);
    QColor valueColor(int value, int lo = -10, int hi = 10) const;
    int m_rows, m_cols;
    bool m_editable;
    bool m_readOnly;
    QVector<QVector<int>> m_data;
    QGridLayout *m_grid;
    QVector<QVector<QWidget*>> m_cells;
    QVector<QVector<QLineEdit*>> m_entries;
    QVector<QVector<QLabel*>> m_labels;
    double m_zoomFactor = 1.0;
};

class ConvolutionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConvolutionWidget(QWidget *parent = nullptr);

private slots:
    void onParamChanged();
    void runConvolution();
    void resetDemo();
    void animationStep();

private:
    void initMatrices();
    void clearAnimation();
    void rebuildInputGrid(int paddedSize, int inputSize, int pad, int cellSz);
    void editInputCell(int r, int c, int pad, int cellSz);
    void restoreInputCell(int r, int c, int pad);
    QString cellBgColor(int val, int lo = -10, int hi = 10);
    QString cellFgColor(int val, int lo = -10, int hi = 10);

    // Controls
    QLabel *m_title;
    QLabel *m_desc;
    QFrame *m_ctrlFrame;
    QComboBox *m_inputSizeCombo;
    QComboBox *m_kernelSizeCombo;
    QComboBox *m_strideCombo;
    QComboBox *m_paddingCombo;
    QPushButton *m_runBtn;
    QPushButton *m_resetBtn;
    QLabel *m_stepInfo;
    QLabel *m_inputLabel;
    QLabel *m_kernelLabel;
    QLabel *m_outputLabel;

    // Input matrix (custom padded grid)
    QWidget *m_inputGroup;
    QVBoxLayout *m_inputGroupLayout;
    QWidget *m_inputGridContainer;
    QGridLayout *m_inputGrid;
    QVector<QVector<QWidget*>> m_inputCells;
    QVector<QVector<int>> m_inputData;
    int m_currentPad;
    QHBoxLayout *m_matrixAreaLayout;
    QScrollArea *m_scrollArea;

    // Matrices
    MatrixGrid *m_kernelMatrix;
    MatrixGrid *m_outputMatrix;

    // Animation
    QTimer *m_animTimer;
    int m_animStep;
    int m_animTotal;
    QVector<QVector<int>> m_animOutput;
    int m_animPaddedSize;
    int m_animKs;
    int m_animStride;
    int m_animOutSize;
    int m_animInputSize;

    double m_zoomFactor = 1.0;

    // Helpers
    void applyZoom();
    bool eventFilter(QObject *obj, QEvent *event) override;

public:
    void applyTheme();
    void resetOutputMatrix(int outSize);
};

#endif // CONVOLUTIONWIDGET_H
