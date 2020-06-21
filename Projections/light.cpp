#include "light.h"
#include <QtMath>

Light::Light()
{

}

void Light::setX(double x)
{
    basePos.setCartesian(x, basePos.y(), basePos.z());
}
void Light::setY(double y)
{
    basePos.setCartesian(basePos.x(), y, basePos.z());
}
void Light::setZ(double z)
{
    basePos.setCartesian(basePos.x(), basePos.y(), z);
}

void Light::setColor(QColor color)
{
    this->color = color;
}
void Light::setAmbColor(QColor color)
{
    this->ambColor = color;
}


void Light::setType(LightType type)
{
    this->type = type;
}

void Light::setAz(int az)
{
    this->az = az;
}

void Light::setZe(int ze)
{
    this->ze = ze;
}


void Light::setrA(QColor rA)
{
    this->rA = Point(rA.redF(), rA.greenF(), rA.blueF());
}
void Light::setrD(QColor rD)
{
    this->rD = Point(rD.redF(), rD.greenF(), rD.blueF());
}
void Light::setrS(QColor rS)
{
    this->rS = Point(rS.redF(), rS.greenF(), rS.blueF());
}
void Light::setH(double h)
{
    if(h > 0)
        this->h = h;
}

Point Light::computeAmbient()
{
    Point ambient(ambColor.redF(), ambColor.greenF(), ambColor.blueF());
    return Point::multArray(ambient, rA);

}
Point Light::computeDiffuse(Point l, Point n)
{
    Point lColor(color.redF(), color.greenF(), color.blueF());
    double ln = Point::dotProduct(l,n);
    return Point::multArray(lColor, (rD*ln));
}
Point Light::computeSpecular(Point v, Point r)
{
    Point lColor(color.redF(), color.greenF(), color.blueF());
    double vr = Point::dotProduct(v,r);
    if(vr <= 0)
        return Point();
    return Point::multArray(lColor, rS*qPow(vr, h));
}
