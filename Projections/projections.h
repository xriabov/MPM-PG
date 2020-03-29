#ifndef PROJECTIONS_H
#define PROJECTIONS_H

#include <QMainWindow>

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

private slots:
    void on_azimuth_valueChanged(int az);
    void on_zenith_valueChanged(int ze);
    void on_projectionType_currentIndexChanged(int type);
    void on_distance_valueChanged(int di);
};
#endif // PROJECTIONS_H
