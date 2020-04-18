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
                                Point(1, 0, 0),
                                Point(0, 1, 0),
                                Point(1, 1, 0),
                                Point(0, 0, 1),
                                Point(1, 0, 1),
                                Point(0, 1, 1),
                                Point(1, 1, 1)
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

    viewerWidget->projection();
}

Projections::~Projections()
{
    delete ui;
}

void Projections::on_azimuth_valueChanged(int az)
{
    viewerWidget->setAzimuth(az);
    viewerWidget->projection();
}
void Projections::on_zenith_valueChanged(int ze)
{
    viewerWidget->setZenith(ze);
    viewerWidget->projection();
}
void Projections::on_projectionType_currentIndexChanged(int type)
{
    if(type == 0)
        viewerWidget->setProjection(ProjectionType::PARALLEL);
    else if(type == 1)
        viewerWidget->setProjection(ProjectionType::CENTER);
    viewerWidget->projection();
}
void Projections::on_distance_valueChanged(int di)
{
    viewerWidget->setDistance(di);
    viewerWidget->projection();
}
void Projections::on_actionGenerate_sphere_triggered()
{
    bool status;
    int w = QInputDialog::getInt(this, "Width resolution", "Enter width resolution of a sphere:", 10, 3, 100000, 1, &status);
    if(!status)
        return;

    int h = QInputDialog::getInt(this, "Height resolution", "Enter height resolution of a sphere:", 10, 3, 100000, 1, &status);
    if(!status)
        return;

    int r = QInputDialog::getInt(this, "Radius", "Enter sphere's radius:", 10, 5, 100000, 1, &status);
    if(!status)
        return;

    QString fileName = QFileDialog::getSaveFileName(this, "Save vtk file", "./");
    if(fileName.isNull() || fileName.isEmpty())
        return;

    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);

    // VTK Header

    out << "# vtk DataFile Version 2.0 \n"
        << "vtk output\n"
        << "ASCII\n"
        << "DATASET POLYDATA\n";

    // Points

    out << "POINTS " << w * (h-1) + 2 << " float\n"
        << "0.0 0.0 " << r << "\n";
    double x, y, z, az, ze, zeSinRadius, zeCosRadius;
    double azStep = 2*M_PI/w;
    double zeStep = M_PI/h;
    for(ze = zeStep; ze < zeStep*h; ze += zeStep)
    {
        zeSinRadius = r * qSin(ze);
        zeCosRadius = r * qCos(ze);
        for(az = 0; az < azStep*w; az += azStep)
        {
            x = zeSinRadius * qCos(az);
            y = zeSinRadius * qSin(az);
            z = zeCosRadius;

            out << x << " " << y << " " << z << "\n";
        }
    }

    out << "0.0 0.0 " << -r << "\n";
    // Edges
    int points = w * (h-1) + 2;
    int lineCount = w * (h-1) * 3;
    int a,b,c,d;

    out << "LINES " << lineCount << " " << lineCount*3 << "\n";

    for(int i = 1; i < points-1; i++)

    {
        a = i - w;
        if(a < 0)
            a = 0;
        b = i;
        if(i % w == 0)
        {
            c = i - w + 1;
            d = i + 1;
        } else
        {
            c = i + 1;
            d = i + w + 1;
        }
        if(d >= points)
            d = points-1;
        out << "2 " << a << " " << b << "\n";
        out << "2 " << b << " " << c << "\n";
        out << "2 " << b << " " << d << "\n";
    }


    // Polygons
    int polyCount = w * (h-1) * 2;
    out << "POLYGONS " << polyCount << " " << polyCount * 4 << "\n";

    for(int i = 1; i < points-1; i++)
    {
        a = i - w;
        if(a < 0)
            a = 0;
        b = i;
        if(i % w == 0)
        {
            c = i - w + 1;
            d = i + 1;
        } else
        {
            c = i + 1;
            d = i + w + 1;
        }
        if(d >= points)
            d = points-1;
        out << "3 " << b << " " << a << " " << c << "\n";
        out << "3 " << b << " " << c << " " << d << "\n";
    }

    // Close file
    file.close();
}

void Projections::on_actionOpen_file_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open .vtk file", QString(), "*.vtk");
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);

    if(!in.readLine().contains("# vtk DataFile Version"))
        return;
    in.readLine();
    in.readLine();
    in.readLine();

    QString str;
    QStringList split;
    QList<Point> Points;
    QList<Edge> Edges;
    QList<QList<int>> Polygons;
    int num;

    while(true)
    {
        str = in.readLine();
        if(str == "\n")
            continue;
        else if(str.isEmpty())
            break;

        split = str.split(" ");
        if(split[0] == "POINTS")
        {
            num = split[1].toInt();
            for(int i = 0; i < num; i++)
            {
                str = in.readLine();
                split = str.split(" ");
                Points.append(Point(split[0].toDouble(), split[1].toDouble(), split[2].toDouble()));
            }
            viewerWidget->loadPoints(Points);
        } else if(split[0] == "POLYGONS")
        {
            num = split[1].toInt();
            for(int i = 0; i < num; i++)
            {
                str = in.readLine();
                split = str.split(" ");
                Polygons.append(QList<int>());

                for(int j = 0; j < split[0].toInt(); j++)
                    Polygons.last().append(split[j+1].toInt());
            }
            viewerWidget->loadPolygons(Polygons);
        } else if(split[0] == "LINES")
        {
            num = split[1].toInt();
            for(int i = 0; i < num; i++)
            {
                str = in.readLine();
                split = str.split(" ");
                Edges.append(Edge());

                Edges.last().firstP = split[1].toInt();
                Edges.last().secondP = split[2].toInt();
            }
            viewerWidget->loadEdges(Edges);
        }
    }
    viewerWidget->projection();
}
