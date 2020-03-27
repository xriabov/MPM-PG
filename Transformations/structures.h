#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <QPoint>

struct Ellipse
{
    QPoint point;
    int a, b;
};

struct Circle
{
    QPoint center;
    int r;
};

#endif // STRUCTURES_H
