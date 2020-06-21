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
    void on_displayBox_currentIndexChanged(int type);
    void on_shadingBox_currentIndexChanged(int type);

    void on_worldX_valueChanged(double x);
    void on_worldY_valueChanged(double y);
    void on_worldZ_valueChanged(double z);
    void on_worldAz_valueChanged(int az);
    void on_worldZe_valueChanged(int ze);
    void on_worldScale_valueChanged(double scale);

    void on_xCPos_valueChanged(double x);
    void on_yCPos_valueChanged(double y);
    void on_zCPos_valueChanged(double z);
    void on_azimuth_valueChanged(int az);
    void on_zenith_valueChanged(int ze);

    void on_projectionType_currentIndexChanged(int type);
    void on_distance_valueChanged(int di);

    void on_actionGenerate_sphere_triggered();
    void on_actionOpen_file_triggered();

    void on_lightId_valueChanged(int i);
    void on_lightAdd_clicked();

    void on_xLPos_valueChanged(double x);
    void on_yLPos_valueChanged(double z);
    void on_zLPos_valueChanged(double y);

    void on_azimuthL_valueChanged(int az);
    void on_zenithL_valueChanged(int ze);

    void on_lightType_currentIndexChanged(int id);

    void on_colorButton_clicked();
    void on_ambColorButton_clicked();

    void on_ra_clicked();
    void on_rd_clicked();
    void on_rs_clicked();
    void on_h_valueChanged(double h);
};
#endif // PROJECTIONS_H
