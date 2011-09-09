#include <QtGui/QApplication>
#include "globals.h"
#include "application.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    WoWModelViewer w;
	g_WMV = &w;
    w.show();

    return app.exec();
}
