#include "widget.h"
#include "Graph3DWindow.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    SensorManager* manager = new SensorManager();

    Widget w(manager);
    w.resize(1280, 720);
    w.show();

    Graph3DWindow* ventanaGeneral = new Graph3DWindow(manager);
    ventanaGeneral->resize(1000, 500);
    ventanaGeneral->show();

    return a.exec();
}
