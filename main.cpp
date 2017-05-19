#include "view/mainwindow.h"

#include <QApplication>

///
/// \brief Main function of this application.
///
/// \param [in] argc    The number of strings pointed to by argv.
/// \param [in] argv    Name of the programm + misc.
///
/// \return             0 if the application ran successfully, otherwise an error code.
///
int main(int argc, char *argv[])
{
    // create application
    QApplication app(argc, argv);
    app.setOrganizationName("The University of Queensland");
    app.setApplicationName("SMILX_VP");

    // create the main window and enter the main execution loop
    MainWindow window;
    window.show();
    int result = app.exec();

    return result;
}
