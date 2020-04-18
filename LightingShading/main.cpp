#include "ls.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LS w;
    w.show();
    return a.exec();
}
