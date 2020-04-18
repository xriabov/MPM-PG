﻿#include "viewer.h"


// // Point

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

//  // Viewer

Viewer::Viewer(QSize size, QWidget *parent):
    QWidget(parent)
{
    this->size = size;
    this->setObjectName("ImageViewer");
    img = new QImage(size, QImage::Format_ARGB32);
    img->fill(Qt::white);

    this->setMinimumSize(size);
    this->setMaximumSize(size);
    this->resize(size);
    painter = new QPainter(img);

    setCameraPos(Point(0,0,0));
}

Viewer::~Viewer()
{
    painter->end();
    delete painter;
    delete img;
}

void Viewer::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QRect area = event->rect();
    painter.drawImage(area, *img, area);
}

void Viewer::projection()
{
    Points = basePoints;

    double* matrix;
    matrix = worldTransform(-0.5, -0.5, 1);
    applyTransformation(matrix);
    delete[] matrix;

    calcCameraVectors();

    if(type == ProjectionType::PARALLEL)
        matrix = camTranspose();
    else if(type == ProjectionType::CENTER)
        matrix = camTransposeCenter();
    applyTransformation(matrix);
    delete[] matrix;

    matrix = camZ();
    applyTransformation(matrix);
    delete[] matrix;

    matrix = camX();
    applyTransformation(matrix);
    delete[] matrix;

    if(type == ProjectionType::PARALLEL)
        matrix = projectParallel();
    else if(type == ProjectionType::CENTER)
        matrix = projectCenter();

    applyTransformation(matrix);
    delete[] matrix;

    // Clip
    // Viewport
    viewportTransform();

    if(!Edges.isEmpty())
        printLines();
    //if(!Polygons.isEmpty())
    //    printPoly();
}

double* Viewer::worldTransform(double x, double y, double z)
{
    return  new double[16] {
        1.,0.,0.,0.,
        0.,1.,0.,0.,
        0.,0.,1.,0.,
        x, y, z, 1.
    };
}


double* Viewer::projectParallel()
{
    double c = 0.2;
    double f = 10;

    return new double[16] {
        1., 0, 0, 0,
        0, 1., 0, 0,
        0, 0, -1./(f-c), 0,
        0, 0, -c/(f-c), -1
    };
}

double* Viewer::projectCenter()
{
    double FoV = 60;
    double c, f;
    double h;
    h = 1/qTan(FoV*M_PI/360);
    c = 0.2;
    f = 5;
    return new double[16] {
        di, 0, 0, 0,
        0, di, 0, 0,
        0, 0, -f/(f-c), -1.,
        0, 0, -(c*f)/(f-c), 0
    };
}

double* Viewer::cameraTransform()
{
    Point x(camera.v3), y(camera.v2), z(camera.v1);
    double dx, dy, dz;
    dx = camera.pos.x();
    dy = camera.pos.y();
    dz = camera.pos.z();
/*    return new double[16] {
            x.x(), y.x(), z.x(), 0,
            x.y(), y.y(), z.y(), 0,
            x.z(), y.z(), z.z(), 0,
            dx, dy, dz, 1
    };*/
    return new double[16] {
            x.x(), x.y(), x.z(), 0,
            y.x(), y.y(), y.z(), 0,
            z.x(), z.y(), z.z(), 0,
            dx,    dy,    dz,    1
    };

}

double* Viewer::camTranspose()
{
    Point p = camera.pos;
    return new double[16] {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        -p.x(), -p.y(), -p.z(), 1
    };
}
double* Viewer::camTransposeCenter()
{
    Point p;
    p.setSpherical(di, ze, az);
    p = camera.pos - p;
    return new double[16] {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        -p.x(), -p.y(), -p.z(), 1
    };
}

double* Viewer::camX()
{
    double zenith = qDegreesToRadians(ze);
    return new double[16] {
        1, 0, 0, 0,
        0, qCos(zenith), -qSin(zenith), 0,
        0, qSin(zenith), qCos(zenith), 0,
        0, 0, 0, 1
    };
}
double* Viewer::camZ()
{
    double azimuth = qDegreesToRadians(az);
    return new double[16] {
        qCos(azimuth), -qSin(azimuth), 0, 0,
        qSin(azimuth), qCos(azimuth), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

double* Viewer::clipping()
{
    // Not implemented yet
    return new double[16] {};
}

void Viewer::viewportTransform()
{
    QSize size = QSize(this->size.width(), this->size.height());
    for(int i = 0; i < Points.length(); i++)
    {
        Points[i].setCartesian(
                    (Points[i].x() + 2.)*size.width()/4,
                    (-Points[i].y() + 2.)*size.height()/4,
                    Points[i].z()
                    );
    }
}


void Viewer::printLines()
{
    img->fill(Qt::white);
    for(int i = 0; i < Edges.length(); i++)
    {
        Point first = Points[Edges[i].firstP];
        Point second = Points[Edges[i].secondP];
        if(isinf(first.rz()) || isinf(second.rz()) || first.z() < -2 || second.z() < -2)
            continue;

        printLine(first, second);
    }
    this->update();
}

void Viewer::printPoly()
{
    img->fill(Qt::white);
    QList<Point> points;
    for(int i = 0; i < Polygons.length(); i++)
    {
        for(int j = 0; j < Polygons[i].length(); j++)
            points.append(Points[Polygons[i][j]]);
        printPolygon(points);
        points.clear();
    }
}

void Viewer::printLine(Point& p1, Point& p2)
{
    int x, y, xi, yi, dx, dy, d, dDecrement, dIncrement, xMax, yMax;
    x = static_cast<int>(p1.rx());
    y = static_cast<int>(p1.ry());

    dx = abs(static_cast<int>(p2.rx()) - static_cast<int>(p1.rx()));
    dy = abs(static_cast<int>(p2.ry()) - static_cast<int>(p1.ry()));
    xi = static_cast<int>(p2.rx()) - static_cast<int>(p1.rx()) >= 0 ? 1 : -1;
    yi = static_cast<int>(p2.ry()) - static_cast<int>(p1.ry()) >= 0 ? 1 : -1;

    bool xAxis = dx > dy;
    d = xAxis ? 2*dy - dx : 2*dx - dy;
    dDecrement = xAxis ? 2*dx : 2*dy;
    dIncrement = xAxis ? 2*dy : 2*dx;

    xMax = img->size().rwidth();
    yMax = img->size().rheight();

    while(x != static_cast<int>(p2.rx()) || y != static_cast<int>(p2.ry()))
    {
        if(x > 0 && x < xMax && y > 0 && y < yMax)
            img->setPixel(x, y, Qt::black);

        if(d > 0)
        {
            d -= dDecrement;
            x += xi;
            y += yi;
        } else{
            if(xAxis)
                x += xi;
            else
                y += yi;
        }
        d += dIncrement;
    }
}

void Viewer::printPolygon(QList<Point> points)
{
    if(points.length() != 3)
        return;
    // scanline for triangle

}

void Viewer::applyTransformation(double* matrix)
{
    for(int i = 0; i < Points.length(); i++)
    {
        Points[i].transform(matrix);
        Points[i].homogeneousToCartesian();
    }
}

void Viewer::setProjection(ProjectionType type)
{
    this->type = type;
}

void Viewer::setAzimuth(int azimuth)
{
    if(azimuth >= 0 && azimuth <= 360)
        az = azimuth;
}

void Viewer::setZenith(int zenith)
{
    if(zenith >= 0 && zenith <= 180)
        ze = zenith;
}

void Viewer::setDistance(int distance)
{
    di = distance;
}


void Viewer::loadPoints(QList<Point> points)
{
    this->basePoints = points;

    Edges.clear();
    Polygons.clear();

    double maxCoord = 0;
    for(int i = 0; i < points.length(); i++)
    {
        if(abs(points[i].x()) > maxCoord)
            maxCoord = abs(points[i].x());
        if(abs(points[i].y()) > maxCoord)
            maxCoord = abs(points[i].y());
        if(abs(points[i].z()) > maxCoord)
            maxCoord = abs(points[i].z());
    }

    for(int i = 0; i < points.length(); i++)
    {
        basePoints[i] = basePoints[i] * (1/maxCoord);
    }

    this->Points = basePoints;
}

void Viewer::loadEdges(QList<Edge> edges)
{
    this->Edges = edges;
}

void Viewer::loadPolygons(QList<QList<int>> polygons)
{
    this->Polygons = polygons;
}

void Viewer::setCameraPos(Point pos)
{
    camera.pos = pos;
}

void Viewer::calcCameraVectors()
{
    camera.v1.setSpherical(1, az, ze);
    camera.v1 = Point::norm(camera.pos - camera.v1);
    camera.v2 = Point::norm(Point::vectorProduct(Point::vectorProduct(camera.v1, Point(1,0,0)), camera.v1));
    camera.v3 = Point::norm(Point::vectorProduct(camera.v1, camera.v2));
}
