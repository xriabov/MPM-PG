#ifndef VIEWER_H
#define VIEWER_H

#include <QtWidgets>
#include "point.h"
#include "light.h"

enum struct ProjectionType
{
    PARALLEL = 1,
    CENTER = 2
};

enum struct DisplayType
{
    WIREFRAME = 1,
    POLYGON = 2,
};

enum struct ShadingType
{
    FLAT = 1,
    GOURAUD = 2,
};

struct Edge
{
    int firstP, secondP;
};

struct Camera
{
    Point pos;
    Point eye;
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
    QList<QColor> colors;


    double az = 0.;
    double ze = 0.;
    double di = 1.;

    ProjectionType type = ProjectionType::PARALLEL;

    Camera camera;

    QList<Light> light;

    float** zBuffer;
    QRgb** zImage;

    void printLine(Point& p1, Point& p2, QColor& c1, QColor& c2);
    void printPolygonGouraud(QList<Point> points, QList<QColor> colors);
    void printPolygonFlat(QList<Point> points, QList<QColor> colors);

    double* projectParallel();
    double* projectCenter();

    double* worldTransform(double x, double y, double z, int az, int ze, double scale);
    void calcCameraVectors();
    double* cameraTransform();
    void applyTransformation(double* matrix);

    void transformLight(double* matrix);

    double* camTranspose();
    double* camX();
    double* camZ();

    double* clipping();

    void viewportTransform();

    void printLines();
    void printPoly();

    void fillBuffer();
    void printBuffer();

    void reflectionModel();

public:

    Viewer(QSize size, QWidget *parent = nullptr);
    ~Viewer() Q_DECL_OVERRIDE;


    DisplayType displayType = DisplayType::POLYGON;
    ShadingType shadingType = ShadingType::GOURAUD;

    double worldX = 0.;
    double worldY = 0.;
    double worldZ = 0.;
    int worldAz = 0.;
    int worldZe = 0.;
    double worldScale = 1.;

    void setXCamera(double x);
    void setYCamera(double y);
    void setZCamera(double z);
    void setAzimuth(int azimuth);
    void setZenith(int zenith);

    inline QList<Light>& getLight() { return light; }

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
