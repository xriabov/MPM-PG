#ifndef LIGHT_H
#define LIGHT_H

#include "point.h"

enum LightType
{
    POINT = 1,
    PARALLEL = 2
};

class Light
{
public:
    Light();
    void setPoint(Point pos);
    void setType(LightType type);
    void setAngles(int az, int ze);

private:
    Point pos;
    LightType type;

    int az;
    int ze;
};

#endif // LIGHT_H
