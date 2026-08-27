#pragma once

#include <QWidget>
#include <QVector>

class ChartWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChartWidget(QWidget *parent = nullptr);
    void appendSample(int value);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<int> m_samples;
    int m_maxSamples;
};
