#include <QApplication>
#include "dockerdaemon.h"
#include "constants.h"

QApplication* gApp;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    gApp = &app;

    app.setOrganizationName(Constants::ORG_NAME);
    app.setOrganizationDomain(Constants::DOM_NAME);
    app.setApplicationName(Constants::APP_NAME);
    app.setApplicationVersion(Constants::APP_VERSION);

    DockerDaemon d;

    app.setQuitOnLastWindowClosed(false);

    return app.exec();
}
