#include "ChartWidget.h"
#include <QPainter>
#include <QPaintEvent>

ChartWidget::ChartWidget(QWidget *parent)
    : QWidget(parent), m_maxSamples(60)
{
    setMinimumHeight(150);
}

void ChartWidget::appendSample(int value)
{
    if (m_samples.size() >= m_maxSamples) {
        m_samples.pop_front();
    }
    m_samples.push_back(value);
    update();
}

void ChartWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    p.fillRect(rect(), QColor(30, 30, 30));
    if (m_samples.isEmpty()) return;

    int w = width();
    int h = height();
    int n = m_samples.size();
    int maxv = 1;
    for (int v : m_samples) if (v > maxv) maxv = v;

    QPen pen(QColor(0, 180, 0));
    pen.setWidth(2);
    p.setPen(pen);

    // draw grid
    p.setPen(QColor(70,70,70));
    for (int i=1;i<=4;i++) {
        int y = h*i/5;
        p.drawLine(0, y, w, y);
    }

    // draw polyline
    QPainterPath path;
    for (int i = 0; i < n; ++i) {
        double x = (double)i / qMax(1, n-1) * (w-10) + 5;
        double y = h - (double)m_samples[i] / maxv * (h-10) - 5;
        QPointF pt(x, y);
        if (i == 0) path.moveTo(pt); else path.lineTo(pt);
    }
    p.setPen(pen);
    p.drawPath(path);
}
