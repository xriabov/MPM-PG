#ifndef RASTERIZATIONALGOS_H
#define RASTERIZATIONALGOS_H

#include <QImage>
#include <QPoint>
#include <QColor>

class Rasterization
{
public:
    Rasterization();

    // Lines
    static void lineDDA(QImage *img, QPoint &point1, QPoint &point2, QColor &color);
    static void lineBresenham(QImage *img, QPoint &point1, QPoint &point2, QColor &color);

    // Circle
    static void circleBresenham(QImage *img, QPoint &point1, QPoint &point2, QColor &color);
};
#endif // RASTERIZATIONALGOS_H
