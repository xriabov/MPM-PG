#include "viewer.h"


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

Point& Point::operator -(const Point& vect)
{
    this->xPos -= vect.xPos;
    this->yPos -= vect.yPos;
    this->zPos -= vect.zPos;
    return *this;
}

Point& Point::operator +(const Point& vect)
{
    this->xPos += vect.xPos;
    this->yPos += vect.yPos;
    this->zPos += vect.zPos;
    return *this;
}

// // Viewer

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


void Viewer::projectionParallel()
{
    //V// Compute projection plane
    // //
    // // Project every point to this plane
    // // Print corresponding edges using projected points
    Points = basePoints;

    calcCameraVectors();

    double* matrix = cameraTransform();
    applyTransformation(matrix);
    delete[] matrix;

    matrix = projectParallel();
    applyTransformation(matrix);
    delete[] matrix;

    print();
}

void Viewer::projectionCenter()
{
    Points = basePoints;

    calcCameraVectors();

    double* matrix;

    matrix = cameraTransform();
    applyTransformation(matrix);
    delete[] matrix;

    matrix = projectCenter();
    applyTransformation(matrix);
    delete[] matrix;



    print();
}


double* Viewer::projectParallel()
{
    double FoV = 2*M_PI/3;
    double c, f;
    double w, h;
    w = 2.;
    h = w/(16/9);
    c = 0.1;
    f = 1;
    return new double[16] {
        2.0/w, 0, 0, 0,
        0, 2.0/h, 0, 0,
        0, 0, 1.0/(c-f), -1,
        0, 0, c/(c-f), 0
    };
}

double* Viewer::projectCenter()
{
    double FoV = 2*M_PI/3;
    double c, f;
    double w, h;
    h = 1/qTan(FoV/2.);
    w = h / (16/9);
    c = 0.1;
    f = 1;
    return new double[16] {
        w, 0, 0, 0,
        0, h, 0, 0,
        0, 0, f/(c-f), -1,
        0, 0, (c*f)/(c-f), 0
    };
}

double* Viewer::cameraTransform()
{
    return new double[16] {
            camera.v1.x(), camera.v2.x(), camera.v3.x(), 0,
            camera.v1.y(), camera.v2.y(), camera.v3.y(), 0,
            camera.v1.z(), camera.v2.z(), camera.v3.z(), 0,
            camera.pos.x(), camera.pos.y(), camera.pos.z(), 1};
}


// TODO
void Viewer::print()
{
    img->fill(Qt::white);
    for(int i = 0; i < Edges.length(); i++)
    {
        Point first = Points[Edges[i].firstP];
        Point second = Points[Edges[i].secondP];
        printLine(first, second);
    }
    this->update();
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

    int xC, yC;
    xC = xMax/2;
    yC = yMax/2;

    while(x != static_cast<int>(p2.rx()) || y != static_cast<int>(p2.ry()))
    {
        if(x + xC > 0 && x + xC < xMax && y + yC > 0 && y + yC < yMax)
            img->setPixel(x + xC, y + yC, Qt::black);

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

void Viewer::applyTransformation(double* matrix)
{
    for(int i = 0; i < Points.length(); i++)
    {
        Points[i].transform(matrix);
        Points[i].homogeneousToCartesian();
    }
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
    this->Points = points;
    this->basePoints = points;
}

void Viewer::loadEdges(QList<Edge> edges)
{
    this->Edges = edges;
}


void Viewer::setCameraPos(Point pos)
{
    camera.pos = pos;
}

void Viewer::calcCameraVectors()
{
    camera.v1.setSpherical(1, az, ze);
    camera.v1 = Point::norm(camera.v1);
    camera.v2 = Point::norm(Point::vectorProduct(Point::vectorProduct(camera.v1, Point(0,1,0)), camera.v1));
    camera.v3 = Point::norm(Point::vectorProduct(camera.v1, camera.v2));
}
