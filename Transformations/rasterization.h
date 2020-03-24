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
    static void lineBresenhamLow(QImage *img, QPoint &point1, QPoint &point2, QColor &color);
    static void lineBresenhamHigh(QImage *img, QPoint &point1, QPoint &point2, QColor &color);
};

#endif // RASTERIZATIONALGOS_H
