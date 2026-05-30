#ifndef OVERVIEWWIDGET_H
#define OVERVIEWWIDGET_H

#include <QWidget>
#include <QPainter>

class OverviewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OverviewWidget(QWidget *parent = nullptr);
    void applyTheme();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void rebuildContent();
    QWidget *m_contentWidget;
    QWidget *m_canvas;
};

#endif // OVERVIEWWIDGET_H
