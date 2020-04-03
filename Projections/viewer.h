#ifndef VIEWER_H
#define VIEWER_H

#include <QtWidgets>

class Point
{
private:
    double xPos, yPos, zPos, w = 1;

public:
    inline Point() {}
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

    void transform(double a[16]);

    static Point vectorProduct(const Point& vector1, const Point& vector2);
    static double dotProduct(const Point& vector1, const Point& vector2);
    static double len(const Point& vect);
    static Point norm(const Point& vect);


    Point& operator -(const Point& vect);
    Point& operator +(const Point& vect);
};

struct Edge
{
    int firstP, secondP;
};

struct Camera
{
    Point pos;
    Point v1, v2, v3; // v1 = normal vector, v2 = "up" vector, v3 = vector(v1,v2)
    int angle; // changes v2
};


class Viewer: public QWidget
{
private:

    QSize size;
    QImage *img = nullptr;
    QPainter *painter = nullptr;

    QList<Point> basePoints;
    QList<Point> Points;
    QList<Edge> Edges;

    double az = 0;
    double ze = 0;
    int di = 0;

    Camera camera;

    void printLine(Point& p1, Point& p2);

    double* projectParallel();
    double* projectCenter();

    void calcCameraVectors();
    double* cameraTransform();
    void applyTransformation(double* matrix);

    void print();

public:

    Viewer(QSize size, QWidget *parent = nullptr);
    ~Viewer() Q_DECL_OVERRIDE;

    void setAzimuth(int azimuth);
    void setZenith(int zenith);
    void setDistance(int distance);

    void projectionParallel();
    void projectionCenter();

    void loadPoints(QList<Point> points);
    void loadEdges(QList<Edge> edges);

    // Camera
    void setCameraPos(Point pos);

public slots:
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
};

#endif // VIEWER_H
