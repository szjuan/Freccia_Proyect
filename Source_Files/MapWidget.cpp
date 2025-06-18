#include "MapWidget.h"
#include <QVBoxLayout>
#include <QFile>
#include <QWebEnginePage>

#include <QCoreApplication>
#include <QDir>
#include <QUrl>
#include <QtWebEngineWidgets/QWebEnginePage>

MapWidget::MapWidget(QWidget* parent)
    : QWidget(parent) {
    view = new QWebEngineView(this);
    view->setMinimumSize(500, 300);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(view);

    QString localPath = QDir(QCoreApplication::applicationDirPath()).filePath("assets/map.html");
    view->load(QUrl::fromLocalFile(localPath));
}

void MapWidget::updateMap(double latitude, double longitude) {
    QString js = QString("updatePosition(%1, %2);").arg(latitude, 0, 'f', 8).arg(longitude, 0, 'f', 8);
    view->page()->runJavaScript(js);
}
