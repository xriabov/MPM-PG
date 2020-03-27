#include "structures.h"
#include <QPoint>

Edge::Edge(QPoint point1, QPoint point2)
{
    QPoint begin, end;
    begin = point1;
    end = point2;

    if(begin.ry() > end.ry())
        std::swap(begin, end);

    this->begin = begin;
    this->yEnd = end.ry()-1;
    this->w = static_cast<double>(end.rx() - begin.rx()) / static_cast<double>(end.ry() - begin.ry());
}


bool Edge::operator <(const Edge &edge)
{
    if(this->begin.y() < edge.begin.y())
        return true;
    if(this->begin.y() == edge.begin.y() && this->begin.x() < edge.begin.x())
        return true;
    if(this->begin.y() == edge.begin.y() && this->begin.x() == edge.begin.x() && this->w < edge.w)
        return true;
    return false;
}
