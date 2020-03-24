#include "transformation.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Transformation w;
    w.show();
    return a.exec();
}
