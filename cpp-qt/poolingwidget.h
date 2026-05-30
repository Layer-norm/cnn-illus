#ifndef POOLINGWIDGET_H
#define POOLINGWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QScrollArea>
#include <QWheelEvent>
#include <QFrame>
#include <QVector>
#include "convolutionwidget.h"

class PoolingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PoolingWidget(QWidget *parent = nullptr);

private slots:
    void onParamChanged();
    void runPooling();
    void resetDemo();
    void animationStep();

private:
    void initMatrices();
    void clearAnimation();

    QComboBox *m_inputSizeCombo;
    QComboBox *m_poolTypeCombo;
    QComboBox *m_poolSizeCombo;
    QComboBox *m_strideCombo;
    QPushButton *m_runBtn;
    QPushButton *m_resetBtn;
    QLabel *m_stepInfo;
    QLabel *m_title;
    QLabel *m_desc;
    QFrame *m_ctrlFrame;
    QLabel *m_inputLabel;
    QLabel *m_outputLabel;

    MatrixGrid *m_inputMatrix;
    MatrixGrid *m_outputMatrix;

    QTimer *m_animTimer;
    int m_animStep;
    int m_animTotal;
    QVector<QVector<double>> m_animOutput;
    int m_animPoolSize;
    int m_animStride;
    int m_animOutSize;
    int m_animInputSize;
    bool m_isMaxPool;

    double m_zoomFactor = 1.0;

    // Display area containers
    QScrollArea *m_scrollArea;

    // Helpers
    void applyZoom();
    bool eventFilter(QObject *obj, QEvent *event) override;

public:
    void applyTheme();
    void resetOutputMatrix(int outSize);
};

#endif // POOLINGWIDGET_H
