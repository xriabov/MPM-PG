#ifndef PROJECTIONS_H
#define PROJECTIONS_H

#include <QMainWindow>
#include "viewer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Projections; }
QT_END_NAMESPACE

class Projections : public QMainWindow
{
    Q_OBJECT

public:
    Projections(QWidget *parent = nullptr);
    ~Projections();

private:
    Ui::Projections *ui;
    Viewer* viewerWidget;

private slots:
    void on_xCPos_valueChanged(double x);
    void on_yCPos_valueChanged(double y);
    void on_zCPos_valueChanged(double z);
    void on_azimuth_valueChanged(int az);
    void on_zenith_valueChanged(int ze);

    void on_projectionType_currentIndexChanged(int type);
    void on_distance_valueChanged(int di);

    void on_actionGenerate_sphere_triggered();
    void on_actionOpen_file_triggered();
};
#endif // PROJECTIONS_H
