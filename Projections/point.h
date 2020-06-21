#ifndef POINT_H
#define POINT_H


class Point
{
private:
    double xPos, yPos, zPos, w = 1.;

public:
    inline Point() { xPos = 0.; yPos = 0.; zPos = 0.; }
    Point(double xPos, double yPos, double zPos);
    Point(const Point& point);

    inline double& rx() { return xPos; }
    inline double& ry() { return yPos; }
    inline double& rz() { return zPos; }
    inline double x() { return xPos; }
    inline double y() { return yPos; }
    inline double z() { return zPos; }

    void setSpherical(double p, double az, double ze);
    void setCartesian(double x, double y, double z);

    void homogeneousToCartesian();

    Point transform(double a[16]);

    static Point vectorProduct(const Point& vector1, const Point& vector2);
    static double dotProduct(const Point& vector1, const Point& vector2);
    static double len(const Point& vect);
    static Point norm(const Point& vect);
    static Point multArray(const Point& p1, const Point& p2);

    Point operator -(const Point& vect);
    Point operator +(const Point& vect);
    Point operator *(const double& scalar);

};

#endif // POINT_H
