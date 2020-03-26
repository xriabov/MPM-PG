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

    //Ellipse
    static void ellipseBresenham(QImage *img, QPoint &center, int a, int b, QColor &color);
};
#endif // RASTERIZATIONALGOS_H
