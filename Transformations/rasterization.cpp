#include "rasterization.h"
#include <QtMath>
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

    for(int i = 0; i <= steps; i++)
    {
        if(x > 0 && x < xMax && y > 0 && y < yMax)
            img->setPixel(x, y, colorRgb);
        x += xInc;
        y += yInc;
    }
}

void Rasterization::lineBresenham(QImage *img, QPoint &point1, QPoint &point2, QColor &color)
{
    QRgb colorRgb = color.rgba();
    int x, y, xi, yi, dx, dy, d, dDecrement, dIncrement, xMax, yMax;
    x = point1.rx();
    y = point1.ry();

    dx = abs(point2.rx() - point1.rx());
    dy = abs(point2.ry() - point1.ry());
    xi = point2.rx() - point1.rx() >= 0 ? 1 : -1;
    yi = point2.ry() - point1.ry() >= 0 ? 1 : -1;

    bool xAxis = dx > dy;
    d = xAxis ? 2*dy - dx : 2*dx - dy;
    dDecrement = xAxis ? 2*dx : 2*dy;
    dIncrement = xAxis ? 2*dy : 2*dx;

    xMax = img->size().rwidth();
    yMax = img->size().rheight();

    while(x != point2.rx() || y != point2.ry())
    {
        if(x > 0 && x < xMax && y > 0 && y < yMax)
            img->setPixel(x, y, colorRgb);

        if(d > 0)
        {
            d -= 2 * dDecrement;
            x += xi;
            y += yi;
        } else{
            if(xAxis)
                x += xi;
            else
                y += yi;
        }
        d += 2 * dIncrement;
    }
}

void Rasterization::circleBresenham(QImage *img, QPoint &point1, QPoint &point2, QColor &color)
{
    QRgb colorRgb = color.rgba();
    int x, y, x0, y0, r, d;
    r = static_cast<int>(qSqrt(qPow(point2.rx() - point1.rx(), 2) + qPow(point2.ry() - point1.ry(), 2)));
    x0 = point1.rx();
    y0 = point1.ry();
    x = 0;
    y = r;
    d = 3 - 2*r;

    while(x <= y)
    {
        img->setPixel(x + x0, y + y0, colorRgb);
        img->setPixel(x + x0, -y + y0, colorRgb);
        img->setPixel(y + x0, x + y0, colorRgb);
        img->setPixel(y + x0, -x + y0, colorRgb);
        img->setPixel(-x + x0, y + y0, colorRgb);
        img->setPixel(-x + x0, -y + y0, colorRgb);
        img->setPixel(-y + x0, x + y0, colorRgb);
        img->setPixel(-y + x0, -x + y0, colorRgb);

        if(d > 0)
        {
            d += 4 - 4 * y;
            y--;
        }
        d += 6 + 4*x;
        x++;
    }
}
