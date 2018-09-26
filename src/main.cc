#include <QApplication>
#include "mainwindow.hh"

#if defined(WIN32) && defined(RELEASE)
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(lycam);
    QApplication app(argc, argv);

    MainWindow *mainWindow = new MainWindow();
    mainWindow->show();

    return app.exec();
}
