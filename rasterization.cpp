#include "rasterization.h"

Rasterization::Rasterization()
{

}

void Rasterization::lineDDA(QImage *img, QPoint &point1, QPoint &point2, QColor &color)
{
    if(point1 == point2) return;
    QRgb colorRgb = color.rgba();

    double x, y;
    int dx, dy;
    double xInc, yInc;
    int steps;

    dx = point2.rx() - point1.rx();
    dy = point2.ry() - point1.ry();
    x = point1.rx();
    y = point1.ry();

    steps = abs(dx) >= abs(dy) ? abs(dx) : abs(dy);

    xInc = static_cast<double>(dx)/steps;
    yInc = static_cast<double>(dy)/steps;


    int xMax = img->size().width();
    int yMax = img->size().height();

    for(int i = 0; i < steps; i++)
    {
        if(x > 0 && x < xMax && y > 0 && y < yMax)
            img->setPixel(x, y, colorRgb);
        x += xInc;
        y += yInc;
    }
}

void Rasterization::lineBresenham(QImage *img, QPoint &point1, QPoint &point2, QColor &color)
{

}
