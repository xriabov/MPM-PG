#ifndef VIEWER_H
#define VIEWER_H

#include <QtWidgets>

enum ProjectionType
{
    PARALLEL = 1,
    CENTER = 2
};

class Point
{
private:
    double xPos, yPos, zPos, w = 1.;

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


    Point operator -(const Point& vect);
    Point operator +(const Point& vect);
    Point operator *(const double& scalar);
};


struct Edge
{
    int firstP, secondP;
};

struct Camera
{
    Point pos;
    Point v1, v2, v3; // v1 = normal vector, v2 = "up" vector, v3 = vector(v1,v2)
    double angle; // changes v2
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
    QList<QList<int>> Polygons;

    double az = 0.;
    double ze = 0.;
    double di = 1.;

    ProjectionType type = ProjectionType::PARALLEL;

    Camera camera;

    void printLine(Point& p1, Point& p2);
    void printPolygon(QList<Point> points);

    double* projectParallel();
    double* projectCenter();

    double* worldTransform(double x, double y, double z);
    void calcCameraVectors();
    double* cameraTransform();
    void applyTransformation(double* matrix);

    double* camTranspose();
    double* camTransposeCenter();
    double* camX();
    // double* camY();
    double* camZ();

    double* clipping();

    void viewportTransform();

    void printLines();
    void printPoly();

public:

    Viewer(QSize size, QWidget *parent = nullptr);
    ~Viewer() Q_DECL_OVERRIDE;

    void setAzimuth(int azimuth);
    void setZenith(int zenith);
    void setDistance(int distance);

    void setProjection(ProjectionType type);
    void projection();

    void loadPoints(QList<Point> points);
    void loadEdges(QList<Edge> edges);
    void loadPolygons(QList<QList<int>> polygons);

    // Camera
    void setCameraPos(Point pos);

public slots:
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
};

#endif // VIEWER_H
