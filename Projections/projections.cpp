#include "projections.h"
#include "ui_projections.h"
#include "viewer.h"

Projections::Projections(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Projections)
{
    ui->setupUi(this);
    viewerWidget = new Viewer(ui->frame->size(), ui->frame);
    viewerWidget->loadPoints(QList<Point>{
                                Point(0, 0, 0),
                                Point(100, 0, 0),
                                Point(0, 100, 0),
                                Point(100, 100, 0),
                                Point(0, 0, 100),
                                Point(100, 0, 100),
                                Point(0, 100, 100),
                                Point(100, 100, 100)
                          });

    viewerWidget->loadEdges(QList<Edge> {
                                Edge {0, 1},
                                Edge {0, 2},
                                Edge {1, 3},
                                Edge {2, 3},
                                Edge {4, 5},
                                Edge {4, 6},
                                Edge {5, 7},
                                Edge {6, 7},
                                Edge {0, 4},
                                Edge {1, 5},
                                Edge {2, 6},
                                Edge {3, 7},
                            });

    viewerWidget->projectionParallel();
}

Projections::~Projections()
{
    delete ui;
}

void Projections::on_azimuth_valueChanged(int az)
{
    viewerWidget->setAzimuth(az);
    project();
}
void Projections::on_zenith_valueChanged(int ze)
{
    viewerWidget->setZenith(ze);
    project();
}
void Projections::on_projectionType_currentIndexChanged(int type)
{
    if(type == 0)
        viewerWidget->projectionParallel();
    else if(type == 1)
        viewerWidget->projectionCenter();
}
void Projections::on_distance_valueChanged(int di)
{
    viewerWidget->setDistance(di);
    project();
}

void Projections::project()
{
    if(ui->projectionType->currentIndex() == 0)
        viewerWidget->projectionParallel();
    else if(ui->projectionType->currentIndex() == 1)
        viewerWidget->projectionCenter();
}
