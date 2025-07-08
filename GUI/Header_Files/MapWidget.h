#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QWebEngineView>
#include <QWebChannel>
#include <QtWebEngineWidgets/QWebEngineView>

class MapWidget : public QWidget {
    Q_OBJECT

public:
    explicit MapWidget(QWidget* parent = nullptr);
    void updateMap(double latitude, double longitude);

private:
    QWebEngineView* view;
};

#endif // MAPWIDGET_H
