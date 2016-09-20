#include "mainwindow.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    // create application
    QApplication app(argc, argv);
    app.setApplicationName("VP Plugin");
    app.setOrganizationName("UQ");

    // create the main window and enter the main execution loop
    MainWindow window;
    window.show();
    int result = app.exec();

    return result;
}
