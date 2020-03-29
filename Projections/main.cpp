#include "projections.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Projections w;
    w.show();
    return a.exec();
}
