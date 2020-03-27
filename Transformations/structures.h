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

class Edge
{
private:
    QPoint begin;
    double w;
    int yEnd;
public:
    Edge(QPoint begin, QPoint end);

    inline QPoint getPoint() { return begin; }
    inline double getW() { return w; }
    inline int getYEnd() { return yEnd; }

    bool operator <(const Edge &edge);
};

#endif // STRUCTURES_H
