#include "TrackerWidget/trackerwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TrackerWidget w;
    w.show();
    return a.exec();
}
