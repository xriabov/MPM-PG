#ifndef LIGHT_H
#define LIGHT_H

#include "point.h"
#include <QColor>

enum struct LightType
{
    POINT = 1,
    PARALLEL = 2
};

class Light
{
public:
    Light();
    void setX(double x);
    void setY(double y);
    void setZ(double z);
    void setType(LightType type);
    void setAz(int az);
    void setZe(int ze);

    void setColor(QColor color);
    void setAmbColor(QColor color);

    inline void resetPos() { pos = basePos; }
    inline Point getBase() { return basePos; }
    inline Point getPos() { return pos; }
    inline int getAz() { return az; }
    inline int getZe() { return ze; }
    inline LightType getType() { return type; }
    inline void setPosTransformed(Point pos) { this->pos = pos; }

    Point computeAmbient();
    Point computeDiffuse(Point l, Point n);
    Point computeSpecular(Point v, Point r);

    void setrA(QColor rA);
    void setrD(QColor rD);
    void setrS(QColor rS);
    void setH(double h);

private:
    Point basePos = Point();
    Point pos = Point();
    LightType type = LightType::POINT;

    QColor color;
    QColor ambColor;

    int az;
    int ze;

    Point rA;
    Point rD;
    Point rS;
    double h = 1.;
};

#endif // LIGHT_H
