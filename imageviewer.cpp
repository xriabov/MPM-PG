#include "imageviewer.h"
#include <QtMath>

ImageViewer::ImageViewer(QSize size, QWidget *parent): QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    setMouseTracking(true);

    this->size = size;
    this->setObjectName("ImageViewer");
    img = new QImage(size, QImage::Format_ARGB32);
    img->fill(Qt::white);

    this->setMinimumSize(size);
    this->setMaximumSize(size);
    this->resize(size);
    painter = new QPainter(img);
    transformMatrix = QTransform();
}

void ImageViewer::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QRect area = event->rect();
    painter.drawImage(area, *img, area);
}

ImageViewer::~ImageViewer()
{
    painter->end();
    delete img;
    delete painter;
}

void ImageViewer::addPoint(QPoint point)
{
    basePoints.append(point);
    points.append(point);
}

void ImageViewer::setPainter()
{
    QPen pen;
    pen.setColor(color);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setWidth(4);
    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing);
}

void ImageViewer::drawPoints()
{
    setPainter();

    QPoint point1, point2;
    point1 = points.first();
    for(int i = 0; i < points.length(); i++){
        point2 = points[i];
        (*drawLine)(img, point1, point2, color);
        point1 = point2;
    }
    this->update();
}

void ImageViewer::completePolygon()
{
    setPainter();
    (*drawLine)(img, points.first(), points.last(), color);
    this->update();
}


// Transformations
void ImageViewer::rotateClockwise()
{
    qreal c10 = qCos(M_PI/8);
    qreal s10 = qSin(M_PI/8);
    qreal dx = points.first().rx();
    qreal dy = points.first().ry();

    qreal* elems = new qreal[9]
    {
       c10, -s10, - dx * c10 + dy * s10 + dx,
       s10, c10, - dx * s10 - dy * c10 + dy,
       0, 0, 1,
    };
    applyTransform(elems);
    clear();
    drawPoints();
    completePolygon();
}

void ImageViewer::rotateCounterClockwise()
{
    qreal c10 = qCos(-M_PI/8);
    qreal s10 = qSin(-M_PI/8);
    qreal dx = points.first().rx();
    qreal dy = points.first().ry();

    qreal* elems = new qreal[9]
    {
       c10, -s10, - dx * c10 + dy * s10 + dx,
       s10, c10, - dx * s10 - dy * c10 + dy,
       0, 0, 1,
    };
    applyTransform(elems);
    clear();
    drawPoints();
    completePolygon();
}

void ImageViewer::zoomIn()
{
    qreal dx = points.first().rx();
    qreal dy = points.first().ry();

    qreal* elems = new qreal[9]
    {
       1.1, 0, -0.1*dx,
       0, 1.1, -0.1*dy,
       0, 0, 1,
    };
    applyTransform(elems);
    clear();
    drawPoints();
    completePolygon();
}

void ImageViewer::zoomOut()
{
    qreal dx = points.first().rx();
    qreal dy = points.first().ry();

    qreal* elems = new qreal[9]
    {
       0.9, 0, 0.1*dx,
       0, 0.9, 0.1*dy,
       0, 0, 1,
    };
    applyTransform(elems);
    clear();
    drawPoints();
    completePolygon();
}

void ImageViewer::swap()
{
    qreal tan = static_cast<qreal>(points[1].ry() - points[0].ry()) / static_cast<qreal>(points[1].rx() - points[0].rx());
    qreal atan = qAtan(tan);
    qreal c = qCos(2*atan);
    qreal s = qSin(2*atan);

    qreal dx = points.first().rx();
    qreal dy = points.first().ry();

    qreal* elems = new qreal[9]
    {
       c,   s, (-dx*c - dy*s) + dx,
       s,  -c, (-dx*s + dy*c) + dy,
       0,   0,   1,
    };

    /*
     * First transform
     * 1  0  0  x
     * 0 -1  0  y
     * 0  0  1  1
     *
     * Second transform
     * 1  0 -a
     * 0  1  b
     * 0  0  1
     *
     * ///
     * 1  0 -dx
     * 0 -1  dy
     * 0  0   1
     * ///
     *
     * Third transform
     * c -s  0
     * s  c  0
     * 0  0  1
     *
     * ///
     * c  s -ac-bs
     * s -c -as+bc
     * 0  0    1
     * ///
     *
     * Fourth transform
     * 1  0  a
     * 0  1  b
     * 0  0  1
     *
    */

    applyTransform(elems);
    clear();
    drawPoints();
    completePolygon();
}

void ImageViewer::shearR()
{
    qreal dx = points.first().rx();
    qreal dy = points.first().ry();

    qreal* elems = new qreal[9]
    {
       1, -0.05,0.05*dy,
       0, 1, 0,
       0, 0, 1,
    };
    applyTransform(elems);
    clear();
    drawPoints();
    completePolygon();
}

void ImageViewer::shearL()
{
    qreal dx = points.first().rx();
    qreal dy = points.first().ry();

    qreal* elems = new qreal[9]
    {
       1, 0.05, -0.05*dy,
       0, 1, 0,
       0, 0, 1,
    };
    applyTransform(elems);
    clear();
    drawPoints();
    completePolygon();
}

void ImageViewer::resetTransform()
{
    transformMatrix.reset();
    points = basePoints;
    clear();
    drawPoints();
    completePolygon();
}

void ImageViewer::clear()
{
    img->fill(Qt::white);
    this->update();
}

void ImageViewer::applyTransform(qreal* vals)
{
    for(int i = 0; i < points.length(); i++)
    {
        qreal x = vals[0]*points[i].rx() + vals[1]*points[i].ry() + vals[2];
        qreal y = vals[3]*points[i].rx() + vals[4]*points[i].ry() + vals[5];

        points[i] = QPoint(x,y);
    }
    delete[] vals;
}

void ImageViewer::setColor(QColor color)
{
    if(color.isValid())
        this->color = color;
}

void ImageViewer::clearPoints()
{
    basePoints.clear();
    points.clear();
}


// Transpose

void ImageViewer::moveBegin(QPoint point)
{
    ifMove = true;
    movePoint = point;
}

void ImageViewer::move(QPoint point)
{
    if(!ifMove)
        return;

    qreal dx = point.rx() - movePoint.rx();
    qreal dy = point.ry() - movePoint.ry();
    movePoint = point;
    qreal* elems = new qreal[9]
    {
       1, 0, dx,
       0, 1, dy,
       0, 0, 1,
    };
    applyTransform(elems);
    clear();
    drawPoints();
    completePolygon();
}

void ImageViewer::moveEnd()
{
    ifMove = false;
}


