#include "Viewer.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    Viewer w(16);
    w.resize(1000, 1000);
    w.setAnimating(true);
    w.show();

    return a.exec();
}
