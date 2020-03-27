#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QtWidgets>
#include "rasterization.h"
#include "structures.h"


class ImageViewer : public QWidget
{
    Q_OBJECT
public:
    ImageViewer(QSize size, QWidget *parent = nullptr);
    ~ImageViewer();

    // Polygon
    void addPoint(QPoint point);
    void setColor(QColor color);

    // Drawing
    void drawPoints();
    void completePolygon();
    void clear();
    void clearPoints();
    void setRasterization(int i);

    //Circle
    void circlePoint(QPoint point);
    void drawCircle();
    void clearCircles();

    //Ellipse
    void ellipsePoint(QPoint point, int a, int b);
    void drawEllipse();
    void clearEllipses();

    // Helpers
    inline int getPointsCount() { return basePoints.length(); }

    // Transformations
    void rotateClockwise();
    void rotateCounterClockwise();
    void zoomIn();
    void zoomOut();
    void swap();
    void shearR();
    void shearL();
    void resetTransform();

    inline bool getIfMove() { return ifMove;}
    void moveBegin(QPoint point);
    void move(QPoint point);
    void moveEnd();

private:

    QSize size;
    QImage* img = nullptr;
    QPainter* painter = nullptr;

    QList<QPoint> basePoints;
    QList<QPoint> points;

    QPoint *pointBuffer = nullptr;
    QList<struct Circle> circles;
    QList<struct Ellipse> ellipses;


    QColor color = Qt::black;

    bool ifMove = false;
    QPoint movePoint;

    void applyTransform(qreal* vals);
    void setPainter();

    void (*drawLine)(QImage*, QPoint&, QPoint&, QColor&) = &Rasterization::lineDDA;
    void (*drawC)(QImage*, struct Circle, QColor&) = &Rasterization::circleBresenham;
    void (*drawE)(QImage*, struct Ellipse, QColor&) = &Rasterization::ellipseBresenham;

public slots:
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
};

#endif // IMAGEVIEWER_H
