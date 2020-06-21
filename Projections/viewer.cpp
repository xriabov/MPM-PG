#include "viewer.h"
#include "point.h"

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

    light.append(Light());

    zBuffer = new float*[size.width()];
    zImage = new QRgb*[size.width()];
    for(int i = 0; i < size.width(); i++)
    {
        zBuffer[i] = new float[size.height()];
        zImage[i] = new QRgb[size.height()];
        for(int j = 0; j < size.height(); j++)
        {
            zBuffer[i][j] = 10000000;
            zImage[i][j] = QColor(Qt::white).rgba();
        }

    }

}

Viewer::~Viewer()
{
    for(int i = 0; i < size.width(); i++)
    {
        delete[] zBuffer[i];
        delete[] zImage[i];
    }
    delete[] zBuffer;
    delete[] zImage;


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
    for(int i = 0; i < light.length(); i++)
        light[i].resetPos();

    double* matrix;
    // Rotate, move, scale
    matrix = worldTransform(worldX, worldY, worldZ, worldAz, worldZe, worldScale);
    applyTransformation(matrix);
    delete[] matrix;

    if (displayType == DisplayType::POLYGON)
        reflectionModel();

    matrix = camTranspose();
    applyTransformation(matrix);
    transformLight(matrix);
    delete[] matrix;

    matrix = camZ();
    applyTransformation(matrix);
    transformLight(matrix);
    delete[] matrix;

    matrix = camX();
    applyTransformation(matrix);
    transformLight(matrix);
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

    if(displayType == DisplayType::POLYGON && !Polygons.isEmpty())
        printPoly();
    else if(displayType == DisplayType::WIREFRAME && !Edges.isEmpty())
        printLines();
    else
    {
        img->fill(Qt::white);
        this->update();
    }
}

double* Viewer::worldTransform(double x, double y, double z, int az, int ze, double scale)
{
    double Az = qDegreesToRadians(static_cast<double>(az));
    double Ze = qDegreesToRadians(static_cast<double>(ze));

    double sinA(qSin(Az)),
           sinZ(qSin(Ze)),
           cosA(qCos(Az)),
           cosZ(qCos(Ze));
    return  new double[16] {
        scale * cosA, sinA*cosZ      , sinA*sinZ , 0.,
        -sinA       , scale*cosA*cosZ, cosA*sinZ , 0.,
        0.          , -sinZ          , scale*cosZ, 0.,
        x           , y              , z         , 1.
    };
}

void Viewer::transformLight(double* matrix)
{
    for(int i = 0; i < light.length(); i++)
        light[i].setPosTransformed(light[i].getPos().transform(matrix));
}

double* Viewer::projectParallel()
{
    double c = 0.2;
    double f = 10;
    double scaleY = 1.*size.rwidth()/size.rheight();

    return new double[16] {
        1., 0, 0, 0,
        0, 1.*scaleY, 0, 0,
        0, 0, -1./(f-c), 0,
        0, 0, -c/(f-c), -1
    };
}

double* Viewer::projectCenter()
{
    double FoV = 60;
    double c, f;
    double h;
    double scaleY = 1.*size.rwidth()/size.rheight();
    h = 1/qTan(FoV*M_PI/360);
    c = 0.2;
    f = 5;
    return new double[16] {
        di, 0, 0, 0,
        0, di*scaleY, 0, 0,
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
    Point p;
    p.setSpherical(di, az, ze);
    p = camera.pos - p;
    camera.eye = p;
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
        0, qCos(zenith), qSin(zenith), 0,
        0, -qSin(zenith), qCos(zenith), 0,
        0, 0, 0, 1
    };
}
double* Viewer::camZ()
{
    double azimuth = qDegreesToRadians(az);
    return new double[16] {
        qCos(azimuth), qSin(azimuth), 0, 0,
        -qSin(azimuth), qCos(azimuth), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

void Viewer::reflectionModel()
{
    Point n, l, r, v;

    Point a, d, s, I;
    colors.clear();
    for(int i = 0; i < Points.length(); i++)
    {
        // Vectors
        n = Point::norm(Points[i] - Point(worldX, worldY, worldZ));
        I = Point(0,0,0);
        for(int j = 0; j < light.length(); j++)
        {
        //
            l = Point::norm(light[j].getPos() - Points[i]);
            double ln = Point::dotProduct(l, n);
            if(ln <= 0.)
            {
                continue;
            } else
            {
                r = Point::norm(n*ln*2. - l);
                v = Point::norm(Points[i]*(-1));

                d = light[j].computeDiffuse(l,n);
                s = light[j].computeSpecular(v,r);

                I = I + d + s;
            }
        }

        a = light[0].computeAmbient();
        I = I + a;

        if(I.x() > 1)
            I.setCartesian(1, I.y(), I.z());
        else if(I.x() < 0)
            I.setCartesian(0, I.y(), I.z());
        if(I.y() > 1)
            I.setCartesian(I.x(), 1, I.z());
        else if(I.y() < 0)
            I.setCartesian(I.x(), 0, I.z());
        if(I.z() > 1)
            I.setCartesian(I.x(), I.y(), 1);
        else if(I.z() < 0)
            I.setCartesian(I.x(), I.y(), 0);

        colors.append(QColor(I.x()*255, I.y()*255, I.z()*255));
    }
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

        printLine(first, second, colors[Edges[i].firstP], colors[Edges[i].secondP]);
    }
    this->update();
}

void Viewer::printPoly()
{
    img->fill(Qt::white);
    for(int i = 0; i < size.width(); i++)
    {
        for(int j = 0; j < size.height(); j++)
        {
            zBuffer[i][j] = 10000000;
            zImage[i][j] = QColor(Qt::white).rgba();
        }
    }

    QList<Point> points;
    QList<QColor> color;
    for(int i = 0; i < Polygons.length(); i++)
    {
        for(int j = 0; j < Polygons[i].length(); j++)
        {
            points.append(Points[Polygons[i][j]]);
            color.append(colors[Polygons[i][j]]);
        }
        if(shadingType == ShadingType::GOURAUD)
            printPolygonGouraud(points, color);
        else if(shadingType == ShadingType::FLAT)
            printPolygonFlat(points, color);
        points.clear();
        color.clear();
    }
    printBuffer();
    this->update();
}

void Viewer::printLine(Point& p1, Point& p2, QColor &c1, QColor &c2)
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
            img->setPixel(x, y, c1.rgb());

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

void Viewer::printPolygonFlat(QList<Point> points, QList<QColor> colors)
{
    if(points.length() != 3)
        return;

    // scanline for triangle
    int midI(0), topI(0), botI(0);
    for(int i = 1; i < 3; i++)
    {
        topI = points[i].ry() <= points[topI].ry() ? i : topI;
        botI = points[i].ry() > points[botI].ry() ? i : botI;
    }
    midI = 3 - (topI + botI);

    int xFrom, xTo, yInt;
    double w1,w2,w3;
    double l1, l2, l3;
    double r, g, b;
    float z;
    Point p1(points[topI]), p2(points[midI]), p3(points[botI]);
    QColor polyColor;
    // Barycentric transformation declarations
    Point v0(p2 - p1), v1(p3 - p1), v2;
    double d00(Point::dotProduct(v0, v0)),
           d01(Point::dotProduct(v0, v1)),
           d11(Point::dotProduct(v1, v1)),
           d20, d21,
           denom(d00*d11 - d01*d01);

    w1 = (points[midI].x() - points[topI].x()) / (points[midI].y() - points[topI].y());
    w2 = (points[botI].x() - points[topI].x()) / (points[botI].y() - points[topI].y());
    w3 = (points[botI].x() - points[midI].x()) / (points[botI].y() - points[midI].y());
    r = (colors[0].redF() + colors[1].redF() + colors[2].redF()) / 3;
    g = (colors[0].greenF() + colors[1].greenF() + colors[2].greenF()) / 3;
    b = (colors[0].blueF() + colors[1].blueF() + colors[2].blueF()) / 3;
    for(double y = points[topI].ry(); y < points[midI].ry(); y++)
    {
        if(y < 0) continue;
        else if(y > size.height()) break;
        yInt = static_cast<int>(y);
        xFrom = static_cast<int>(w1*(y-points[topI].ry()) + points[topI].rx());
        xTo = static_cast<int>(w2*(y-points[topI].ry()) + points[topI].rx());

        if(xFrom > xTo)
            std::swap(xFrom,xTo);
        for(int x = xFrom; x <= xTo; x++)
        {
            if(x < 0) continue;
            else if(x > size.width()) break;


            v2 = Point(x,y,0) - p1;
            d20 = Point::dotProduct(v2, v0);
            d21 = Point::dotProduct(v2, v1);
            l2 = (d11 * d20 - d01 * d21) / denom;
            l3 = (d00 * d21 - d01 * d20) / denom;
            l1 = 1. - l2 - l3;

            z = static_cast<float>(l1*p1.z() + l2*p2.z() + l3*p3.z());

            if(zBuffer[x][yInt] < z)
                continue;
            zBuffer[x][yInt] = z;
            zImage[x][yInt] = QColor(r*255,g*255,b*255).rgba();
        }
    }
    for(double y = points[midI].ry(); y < points[botI].ry(); y++)
    {
        if(y < 0) continue;
        else if(y > size.height()) break;
        yInt = static_cast<int>(y);
        xFrom = static_cast<int>(w2*(y - points[topI].ry()) + points[topI].rx());
        xTo = static_cast<int>(w3*(y - points[midI].ry()) + points[midI].rx());
        if(xFrom > xTo)
            std::swap(xFrom,xTo);
        for(int x = xFrom; x <= xTo; x++)
        {
            if(x < 0) continue;
            else if(x > size.width()) break;

            v2 = Point(x,y,0) - p1;
            d20 = Point::dotProduct(v2, v0);
            d21 = Point::dotProduct(v2, v1);
            l2 = (d11 * d20 - d01 * d21) / denom;
            l3 = (d00 * d21 - d01 * d20) / denom;
            l1 = 1. - l2 - l3;

            z = static_cast<float>(l1*p1.z() + l2*p2.z() + l3*p3.z());

            if(zBuffer[x][yInt] < z)
                continue;

            zBuffer[x][yInt] = z;
            zImage[x][yInt] = QColor(r*255,g*255,b*255).rgba();
        }
    }
}

void Viewer::printPolygonGouraud(QList<Point> points, QList<QColor> colors)
{
    if(points.length() != 3)
        return;

    // scanline for triangle
    int midI(0), topI(0), botI(0);
    for(int i = 1; i < 3; i++)
    {
        topI = points[i].ry() <= points[topI].ry() ? i : topI;
        botI = points[i].ry() > points[botI].ry() ? i : botI;
    }
    midI = 3 - (topI + botI);

    int xFrom, xTo, yInt;
    double w1,w2,w3;
    double l1, l2, l3;
    double r, g, b;
    float z;
    Point p1(points[topI]), p2(points[midI]), p3(points[botI]);

    // Barycentric transformation declarations
    Point v0(p2 - p1), v1(p3 - p1), v2;
    double d00(Point::dotProduct(v0, v0)),
           d01(Point::dotProduct(v0, v1)),
           d11(Point::dotProduct(v1, v1)),
           d20, d21,
           denom(d00*d11 - d01*d01);

    w1 = (points[midI].x() - points[topI].x()) / (points[midI].y() - points[topI].y());
    w2 = (points[botI].x() - points[topI].x()) / (points[botI].y() - points[topI].y());
    w3 = (points[botI].x() - points[midI].x()) / (points[botI].y() - points[midI].y());

    for(double y = points[topI].ry(); y < points[midI].ry(); y++)
    {
        if(y < 0) continue;
        else if(y > size.height()) break;
        yInt = static_cast<int>(y);
        xFrom = static_cast<int>(w1*(y-points[topI].ry()) + points[topI].rx());
        xTo = static_cast<int>(w2*(y-points[topI].ry()) + points[topI].rx());
        if(xFrom > xTo)
            std::swap(xFrom,xTo);
        for(int x = xFrom; x <= xTo; x++)
        {
            if(x < 0) continue;
            else if(x > size.width()) break;

            v2 = Point(x,y,0) - p1;
            d20 = Point::dotProduct(v2, v0);
            d21 = Point::dotProduct(v2, v1);
            l2 = (d11 * d20 - d01 * d21) / denom;
            l3 = (d00 * d21 - d01 * d20) / denom;
            l1 = 1. - l2 - l3;

            r = l1*colors[topI].red() + l2*colors[midI].red() + l3*colors[botI].red();
            g = l1*colors[topI].green() + l2*colors[midI].green() + l3*colors[botI].green();
            b = l1*colors[topI].blue() + l2*colors[midI].blue() + l3*colors[botI].blue();

            z = static_cast<float>(l1*p1.z() + l2*p2.z() + l3*p3.z());

            if(zBuffer[x][yInt] < z)
                continue;
            zBuffer[x][yInt] = z;
            zImage[x][yInt] = qRgb(r,g,b);
        }
    }
    for(double y = points[midI].ry(); y < points[botI].ry(); y++)
    {
        if(y < 0) continue;
        else if(y > size.height()) break;
        yInt = static_cast<int>(y);
        xFrom = static_cast<int>(w2*(y - points[topI].ry()) + points[topI].rx());
        xTo = static_cast<int>(w3*(y - points[midI].ry()) + points[midI].rx());
        if(xFrom > xTo)
            std::swap(xFrom,xTo);
        for(int x = xFrom; x <= xTo; x++)
        {
            if(x < 0) continue;
            else if(x > size.width()) break;

            v2 = Point(x,y,0) - p1;
            d20 = Point::dotProduct(v2, v0);
            d21 = Point::dotProduct(v2, v1);
            l2 = (d11 * d20 - d01 * d21) / denom;
            l3 = (d00 * d21 - d01 * d20) / denom;
            l1 = 1. - l2 - l3;

            r = l1*colors[topI].red() + l2*colors[midI].red() + l3*colors[botI].red();
            g = l1*colors[topI].green() + l2*colors[midI].green() + l3*colors[botI].green();
            b = l1*colors[topI].blue() + l2*colors[midI].blue() + l3*colors[botI].blue();

            z = static_cast<float>(l1*p1.z() + l2*p2.z() + l3*p3.z());

            if(zBuffer[x][yInt] < z)
                continue;

            zBuffer[x][yInt] = z;
            zImage[x][yInt] = qRgb(r,g,b);
        }
    }
}

void Viewer::fillBuffer()
{
    for(int i = 0; i < size.width(); i++)
    {
        memset(zBuffer[i], UINT8_MAX, size.height()*sizeof(uint8_t));
        memset(zImage[i], UINT_MAX, size.height()*sizeof(uint));
    }

    QList<Point> points;
    QList<QColor> color;

    int xFrom, xTo;
    double w1,w2,w3;
    double l1, l2, l3, det;
    int r, g, b;
    Point p1, p2, p3;
    int midI, topI, botI;
    int yInt;
    uint8_t z;
    for(int i = 0; i < Polygons.length(); i++)
    {
        for(int j = 0; j < Polygons[i].length(); j++)
        {
            points.append(Points[Polygons[i][j]]);
            color.append(colors[Polygons[i][j]]);
        }

        // Scanline
        midI = 0;
        botI = 0;
        topI = 0;
        for(int i = 1; i < 3; i++)
        {
            topI = points[i].ry() <= points[topI].ry() ? i : topI;
            botI = points[i].ry() > points[botI].ry() ? i : botI;
        }
        midI = 3 - (topI + botI);
        p1 = (points[topI]);
        p2 = (points[midI]);
        p3 = (points[botI]);
        w1 = (points[midI].x() - points[topI].x()) / (points[midI].y() - points[topI].y());
        w2 = (points[botI].x() - points[topI].x()) / (points[botI].y() - points[topI].y());
        w3 = (points[botI].x() - points[midI].x()) / (points[botI].y() - points[midI].y());
        for(double y = points[topI].ry(); y < points[midI].ry(); y++)
        {
            if(y < 0) continue;
            else if(y > size.height()) break;
            yInt = y;
            xFrom = w1*(yInt-points[topI].ry()) + points[topI].rx();
            xTo = w2*(yInt-points[topI].ry()) + points[topI].rx();
            if(xFrom > xTo)
                std::swap(xFrom,xTo);
            for(int x = xFrom; x <= xTo; x++)
            {
                if(x < 0) continue;
                else if(x > size.width()) break;
                l1 = (p2.y()-p3.y())*(x-p3.x()) + (p3.x()-p2.x())*(yInt-p3.y());
                l2 = (p3.y()-p1.y())*(x-p3.x()) + (p1.x()-p3.x())*(yInt-p3.y());
                det = (p2.y()-p3.y())*(p1.x()-p3.x()) + (p3.x()-p2.x())*(p1.y()-p3.y());
                l1 /= det;
                l2 /= det;
                l3 = 1 - l1 - l2;
                r = l1*colors[topI].red() + l2*colors[midI].red() + l3*colors[botI].red();
                g = l1*colors[topI].green() + l2*colors[midI].green() + l3*colors[botI].green();
                b = l1*colors[topI].blue() + l2*colors[midI].blue() + l3*colors[botI].blue();
                z = (l1*p1.z() + l2*p2.z() + l3*p3.z())*UINT8_MAX;
                if(z >= zBuffer[x][yInt])
                    continue;
                zBuffer[x][yInt] = z;
                zImage[x][yInt] = qRgb(r, g, b);
            }
        }
        for(double y = points[midI].ry(); y < points[botI].ry(); y++)
        {
            if(y < 0) continue;
            else if(y > size.height()) break;
            yInt = y;
            xFrom = w2*(yInt - points[topI].ry()) + points[topI].rx();
            xTo = w3*(yInt - points[midI].ry()) + points[midI].rx();
            if(xFrom > xTo)
                std::swap(xFrom,xTo);
            for(int x = xFrom; x <= xTo; x++)
            {
                if(x < 0) continue;
                else if(x > size.width()) break;
                l1 = (p2.y()-p3.y())*(x-p3.x()) + (p3.x()-p2.x())*(yInt-p3.y());
                l2 = (p3.y()-p1.y())*(x-p3.x()) + (p1.x()-p3.x())*(yInt-p3.y());
                det = (p2.y()-p3.y())*(p1.x()-p3.x()) + (p3.x()-p2.x())*(p1.y()-p3.y());
                l1 /= det;
                l2 /= det;
                l3 = 1 - l1 - l2;
                r = l1*colors[topI].red() + l2*colors[midI].red() + l3*colors[botI].red();
                g = l1*colors[topI].green() + l2*colors[midI].green() + l3*colors[botI].green();
                b = l1*colors[topI].blue() + l2*colors[midI].blue() + l3*colors[botI].blue();
                z = (l1*p1.z() + l2*p2.z() + l3*p3.z())*UINT8_MAX;
                if(z >= zBuffer[x][yInt])
                    continue;
                zBuffer[x][yInt] = z;
                zImage[x][yInt] = qRgb(r, g, b);
            }
        }
        // End of scanline

        points.clear();
        color.clear();
    }
}

void Viewer::printBuffer()
{
    //img->fill(Qt::white);
    for(int x = 0; x < size.width(); x++)
        for(int y = 0; y < size.height(); y++)
        {
            QRgb rgb = zImage[x][y];
            img->setPixel(x, y, zImage[x][y]);
        }
}

void Viewer::applyTransformation(double* matrix)
{
    for(int i = 0; i < Points.length(); i++)
    {
        Points[i].transform(matrix);
    }
}

void Viewer::setProjection(ProjectionType type)
{
    this->type = type;
}

void Viewer::setXCamera(double x)
{
    camera.pos.setCartesian(x, camera.pos.y(), camera.pos.z());
}
void Viewer::setYCamera(double y)
{
    camera.pos.setCartesian(camera.pos.x(), y, camera.pos.z());
}
void Viewer::setZCamera(double z)
{
    camera.pos.setCartesian(camera.pos.x(), camera.pos.y(), z);
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
