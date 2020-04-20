#include "point.h"
#include "QtMath"

Point::Point(double x, double y, double z)
{
    xPos = x;
    yPos = y;
    zPos = z;
}

Point::Point(const Point& point)
{
    xPos = point.xPos;
    yPos = point.yPos;
    zPos = point.zPos;
}

void Point::setSpherical(double p, double az, double ze)
{
    xPos = p * qSin(qDegreesToRadians(ze)) * qCos(qDegreesToRadians(az));
    yPos = p * qSin(qDegreesToRadians(ze)) * qSin(qDegreesToRadians(az));
    zPos = p * qCos(qDegreesToRadians(ze));
}

void Point::setCartesian(double x, double y, double z)
{
    xPos = x;
    yPos = y;
    zPos = z;
}

void Point::transform(double *a)
{
    double x, y, z;
    x = a[0] * xPos + a[4] * yPos + a[8] * zPos + a[12];
    y = a[1] * xPos + a[5] * yPos + a[9] * zPos + a[13];
    z = a[2] * xPos + a[6] * yPos + a[10] * zPos + a[14];
    w = a[3] * xPos + a[7] * yPos + a[11] * zPos + a[15];
    xPos = x;
    yPos = y;
    zPos = z;
}

void Point::homogeneousToCartesian()
{
    xPos /= w;
    yPos /= w;
    zPos /= w;
    w /= w;
}

Point Point::vectorProduct(const Point& v1, const Point& v2)
{
    return Point(
                v1.yPos * v2.zPos - v1.zPos * v2.yPos,
                v1.zPos * v2.xPos - v1.xPos * v2.zPos,
                v1.xPos * v2.yPos - v1.yPos * v2.xPos);
}

double Point::dotProduct(const Point& v1, const Point& v2)
{
    return qSqrt(v1.xPos * v2.xPos + v1.yPos * v2.yPos + v1.zPos * v2.zPos);
}

double Point::len(const Point& vect)
{
    return dotProduct(vect, vect);
}

Point Point::norm(const Point& vect)
{
    double length = len(vect);
    return Point(vect.xPos/length, vect.yPos/length, vect.zPos/length);
}

Point Point::operator -(const Point& vect)
{
    return Point
            (
                this->xPos - vect.xPos,
                this->yPos - vect.yPos,
                this->zPos - vect.zPos
            );
}

Point Point::operator +(const Point& vect)
{
    return Point
            (
                this->xPos + vect.xPos,
                this->yPos + vect.yPos,
                this->zPos + vect.zPos
            );
}

Point Point::operator *(const double& scalar)
{
    return Point
            (
                this->xPos * scalar,
                this->yPos * scalar,
                this->zPos * scalar
            );
}
