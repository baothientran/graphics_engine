#include "Viewer.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    Viewer w(4);
    w.resize(1000, 1000);
    w.setAnimating(false);
    w.show();

    return a.exec();
}
