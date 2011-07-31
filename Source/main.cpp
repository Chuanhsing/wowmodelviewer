#include <QtGui/QApplication>
#include "application.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    WoWModelViewer w;
    w.show();

    return app.exec();
}
