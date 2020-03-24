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

    bool xAxis = abs(dx) > abs(dy);
    d = xAxis ? 2*dy - dx : 2*dx - dy;
    dDecrement = xAxis ? 2*dx : 2*dy;
    dIncrement = xAxis ? 2*dy : 2*dx;

    xMax = img->size().rwidth();
    yMax = img->size().rheight();

    while(true)
    {
        if(x > 0 && x < xMax && y > 0 && y < yMax)
            img->setPixel(x, y, colorRgb);

        if(x == point2.rx() && y == point2.ry())
            break;

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
