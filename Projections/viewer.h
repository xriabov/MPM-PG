#ifndef VIEWER_H
#define VIEWER_H

#include <QtWidgets>
#include "point.h"

enum ProjectionType
{
    PARALLEL = 1,
    CENTER = 2
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
    double* camZ();

    double* clipping();

    void viewportTransform();

    void printLines();
    void printPoly();

public:

    Viewer(QSize size, QWidget *parent = nullptr);
    ~Viewer() Q_DECL_OVERRIDE;

    void setXCamera(double x);
    void setYCamera(double y);
    void setZCamera(double z);
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
