#include "Graph3DWindow.h"
#include "SensorData.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtDataVisualization/Q3DTheme>
#include <QtDataVisualization/Q3DScatter>
#include <QtDataVisualization/QScatter3DSeries>
#include <QtDataVisualization/QScatterDataProxy>
#include <QtDataVisualization/QScatterDataItem>

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QEntity>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QMesh>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPhongAlphaMaterial>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QPointLight>


void Graph3DWindow::aplicarEstiloGrafico(QChart* chart, QValueAxis* axisX, QValueAxis* axisY) {
    chart->setBackgroundBrush(QBrush(Qt::black));
    chart->legend()->setLabelColor(Qt::white);
    chart->setTitleBrush(QBrush(Qt::white));

    axisX->setLabelsColor(Qt::white);
    axisX->setTitleBrush(QBrush(Qt::white));
    axisY->setLabelsColor(Qt::white);
    axisY->setTitleBrush(QBrush(Qt::white));
}

Graph3DWindow::Graph3DWindow(SensorManager* manager, QWidget* parent)
    : QWidget(parent), m_sensorManager(manager) {
    setWindowIcon(QIcon("./assets/logo_xae.png"));
    setWindowTitle("FRECCIA_XAE - Gráficas 3D y OSM");
    mainLayout = new QGridLayout(this);
    mainLayout->setSpacing(4);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    

    // === Gráfica 2D con nuevas variables ===
    QChart *chartAll = new QChart();
    chartAll->setMargins(QMargins(0, 0, 60, 0));
    chartAll->setTitle("Datos Generales");

    // Crear series
    seriesLat   = new QLineSeries(); seriesLat->setName("Latitud");
    seriesLon   = new QLineSeries(); seriesLon->setName("Longitud");
    seriesRoll  = new QLineSeries(); seriesRoll->setName("Roll");
    seriesPitch = new QLineSeries(); seriesPitch->setName("Pitch");
    seriesYaw   = new QLineSeries(); seriesYaw->setName("Yaw");
    seriesAlt   = new QLineSeries(); seriesAlt->setName("AltDiff");
    seriesSats  = new QLineSeries(); seriesSats->setName("Satélites");
    seriesHDOP  = new QLineSeries(); seriesHDOP->setName("HDOP");

    // Colores personalizados
    seriesRoll->setColor(QColor("#1f77b4"));
    seriesPitch->setColor(QColor("#2ca02c"));
    seriesYaw->setColor(QColor("#ff7f0e"));
    seriesAlt->setColor(QColor("#9467bd"));
    seriesSats->setColor(QColor("#8c564b")); 
    seriesHDOP->setColor(QColor("#ff0080"));
    seriesLat->setColor(QColor("#bcbd22"));
    seriesLon->setColor(QColor("#d62728"));

    // Puntos visibles
    for (auto s : {seriesRoll, seriesPitch, seriesYaw, seriesAlt, seriesSats, seriesHDOP, seriesLat, seriesLon}) {
        s->setPointsVisible(true);
        chartAll->addSeries(s);
    }

    // Ejes
    axisX1 = new QValueAxis();
    axisX1->setTitleText("Tiempo");
    axisX1->setRange(0, 1);

    axisY1 = new QValueAxis();
    axisY1->setTitleText("Valor");
    axisY1->setRange(0, 1);

    chartAll->addAxis(axisX1, Qt::AlignBottom);
    chartAll->addAxis(axisY1, Qt::AlignLeft);

    for (auto s : {seriesRoll, seriesPitch, seriesYaw, seriesAlt, seriesSats, seriesHDOP, seriesLat, seriesLon})
        s->attachAxis(axisX1), s->attachAxis(axisY1);

    // Aplicar estilo
    aplicarEstiloGrafico(chartAll, axisX1, axisY1);

    // Vista
    chartAllView = new QChartView(chartAll);
    chartAllView->setMinimumSize(1000, 600);
    chartAllView->setContentsMargins(0, 0, 40, 0);

    // Layout contenedor
    auto* layout2DConValores = new QVBoxLayout();
    contenedorValoresArriba = new QGridLayout();
    layout2DConValores->addLayout(contenedorValoresArriba);
    layout2DConValores->addWidget(chartAllView);

    // Contenedor general 2D
    auto* widget2D = new QWidget();
    widget2D->setLayout(layout2DConValores);
    containerGeneral2D = widget2D;

    // === Gráfico 3D ===
    scatterGraph = new Q3DScatter();
    auto theme = scatterGraph->activeTheme();
    theme->setType(Q3DTheme::ThemeRetro);
    theme->setBackgroundColor(Qt::black);
    theme->setLabelTextColor(Qt::white);
    theme->setGridLineColor(Qt::gray);
    theme->setColorStyle(Q3DTheme::ColorStyleUniform);

    mainSeries = new QScatter3DSeries();
    trailSeries = new QScatter3DSeries();
    mainSeries->setItemSize(0.2f);
    trailSeries->setItemSize(0.5f);
    mainSeries->setBaseColor(Qt::blue);
    trailSeries->setBaseColor(Qt::red);

    scatterGraph->addSeries(mainSeries);
    scatterGraph->addSeries(trailSeries);

    scatterGraph->axisX()->setTitle("Longitud");
    scatterGraph->axisY()->setTitle("Tiempo");
    scatterGraph->axisZ()->setTitle("Latitud");
    scatterGraph->axisX()->setTitleVisible(true);
    scatterGraph->axisY()->setTitleVisible(true);
    scatterGraph->axisZ()->setTitleVisible(true);

    container3D = QWidget::createWindowContainer(scatterGraph);
    container3D->setMinimumSize(QSize(460, 500));
    container3D->setAutoFillBackground(true);
    QPalette pal = container3D->palette();
    pal.setColor(QPalette::Window, Qt::white);
    container3D->setPalette(pal);

    // === LLMAP ===
    llmapScene = new QGraphicsScene();
    llmapScene->setSceneRect(-200000, -200000, 400000, 400000);  // escala lógica inicial

    llmap = new QGraphicsView(llmapScene);
    llmap->setMinimumSize(500, 300);
    llmap->setStyleSheet("background-color: black;");
    trayecto = QPainterPath();  // solo inicializa vacío
    pathItem = llmapScene->addPath(trayecto, QPen(Qt::green, 2));
    pathItem->setZValue(-1);  // detrás del punto rojo

   //// === Gyro3D ===
    view3D = new Qt3DExtras::Qt3DWindow();
    view3D->defaultFrameGraph()->setClearColor(QColor("#fffff"));
    container3DModel = QWidget::createWindowContainer(view3D);
    container3DModel->setMinimumSize(500, 300);
    container3DModel->setMaximumSize(500, 300);
    container3DModel->setFocusPolicy(Qt::TabFocus);

    // === Cámara ===
    Qt3DRender::QCamera* camera = view3D->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);

    // === Entidad raíz ===
    rootEntity = new Qt3DCore::QEntity();

    // === Controlador de cámara orbital ===
    Qt3DExtras::QOrbitCameraController* camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    camController->setCamera(camera);

    // === Luz ===
    Qt3DCore::QEntity* lightEntity = new Qt3DCore::QEntity(rootEntity);
    auto* light = new Qt3DRender::QPointLight(lightEntity);
    light->setColor("white");
    light->setIntensity(3.0);
    lightEntity->addComponent(light);

    auto* lightTransform = new Qt3DCore::QTransform();
    lightTransform->setTranslation(QVector3D(20.0f, 20.0f, 20.0f));
    lightEntity->addComponent(lightTransform);

    // === Modelo 3D ===
    Qt3DRender::QMesh* mesh = new Qt3DRender::QMesh();
    mesh->setSource(QUrl::fromLocalFile(QCoreApplication::applicationDirPath() + "/assets/model/CoheteGUI.STL"));

    QTimer::singleShot(1000, [mesh]() {
        if (mesh->status() == Qt3DRender::QMesh::Ready) {
            qDebug() << "✅ Modelo STL cargado correctamente.";
        } else if (mesh->status() == Qt3DRender::QMesh::Error) {
            qDebug() << "❌ Error al cargar el modelo STL.";
        } else {
            qDebug() << "⏳ Modelo STL aún no está listo. Estado:" << mesh->status();
        }
    });

    // === Transformación y Material ===
    rocketTransform = new Qt3DCore::QTransform();
    rocketTransform->setScale(300.0f);
    rocketTransform->setTranslation(QVector3D(0.0f, -1.0f, 0.0f));
    rocketTransform->setRotation(QQuaternion::fromEulerAngles(90, 0, 0));

    qDebug() << "🔍 Transform actual: escala =" << rocketTransform->scale();
    rocketTransform = new Qt3DCore::QTransform();



    Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial(rootEntity);

    // Color principal (difuso)
    material->setDiffuse(QColor(180, 180, 255)); // azul claro

    // Luz ambiental que siempre se ve un poco
    material->setAmbient(QColor(150, 150, 150));

    // Especular (reflejos)
    material->setSpecular(QColor(255, 255, 255)); // blanco

    // Brillo del especular
    material->setShininess(50.0f); // entre 1 y 100\

    // === Entidad que representa el cohete ===
    Qt3DCore::QEntity* rocketEntity = new Qt3DCore::QEntity(rootEntity);
    rocketEntity->addComponent(mesh);
    rocketEntity->addComponent(rocketTransform);
    rocketEntity->addComponent(material);

    // === Posicionar la cámara (fijo o centrado) ===
    QVector3D center = QVector3D(0, 0, 0);  // Suponemos que el modelo está centrado
    float distancia = 100.0f;  // Ajusta según lo que veas en pantalla
    camera->setPosition(QVector3D(0, 0, 20));  // Más lejos para asegurar visibilidad
    camera->setViewCenter(QVector3D(0, 0, 0));

    // === Renderizar ===
    view3D->setRootEntity(rootEntity);

    // === Layout general ===
    mainLayout->addWidget(containerGeneral2D, 0, 0);
    mainLayout->addWidget(container3D,         0, 1);
    mainLayout->addWidget(llmap,   1, 0);
    mainLayout->addWidget(container3DModel, 1, 1);
    setLayout(mainLayout);

    // === Conexión de datos ===
    connect(m_sensorManager, &SensorManager::newSensorData, this, [=](const SensorData& d) {
        seriesLat->append(xIndex2D, d.latitude);
        seriesLon->append(xIndex2D, d.longitude);
        seriesRoll->append(xIndex2D, d.Roll);
        seriesPitch->append(xIndex2D, d.Pitch);
        seriesYaw->append(xIndex2D, d.Yaw);
        seriesAlt->append(xIndex2D, d.AltDiff);
        seriesSats->append(xIndex2D, d.satellites);
        seriesHDOP->append(xIndex2D, d.hdop);

        // === Ajuste dinámico eje X (tiempo) ===
        if (xIndex2D > axisX1->max()) {
            axisX1->setMax(xIndex2D);
        }
        if (xIndex2D < axisX1->min()) {
            axisX1->setMin(xIndex2D);
        }

        // === Ajuste dinámico eje Y ===
        double nuevoMaxY = std::numeric_limits<double>::lowest();
        double nuevoMinY = std::numeric_limits<double>::max();

        for (const auto& serie : {seriesRoll, seriesPitch, seriesYaw, seriesAlt, seriesSats, seriesHDOP, seriesLat, seriesLon}) {
            for (const QPointF& punto : serie->points()) {
                if (punto.y() > nuevoMaxY) nuevoMaxY = punto.y();
                if (punto.y() < nuevoMinY) nuevoMinY = punto.y();
            }
        }

        // Le damos un pequeño margen visual para que no quede pegado
        double margen = (nuevoMaxY - nuevoMinY) * 0.1;
        axisY1->setRange(nuevoMinY - margen, nuevoMaxY + margen);

        ++xIndex2D;

        // Limpiar anteriores
        for (auto item : tooltipTexts) delete item;
        for (auto line : tooltipLines) delete line;
        tooltipTexts.clear();
        tooltipLines.clear();

        int lastIndex = seriesRoll->count() - 1;
        QList<QRectF> ocupados;  // Para controlar colisiones

        for (auto serie : {seriesRoll, seriesPitch, seriesYaw, seriesAlt, seriesSats, seriesHDOP, seriesLat, seriesLon}) {
            if (serie->count() > 0) {
                serie->setPointsVisible(true);
                QPointF punto = serie->at(lastIndex);

                // Unidad
                QString unidad;
                if (serie == seriesAlt)          unidad = " m";
                else if (serie == seriesHDOP)    unidad = "";
                else if (serie == seriesSats)    unidad = " sat";
                else if (serie == seriesRoll ||
                        serie == seriesPitch ||
                        serie == seriesYaw)     unidad = "°";
                else if (serie == seriesLat ||
                        serie == seriesLon)     unidad = "°";

                QString texto = QString("%1%2").arg(QString::number(punto.y(), 'f', 2)).arg(unidad);

                // Texto
                QGraphicsTextItem* etiqueta = new QGraphicsTextItem(texto);
                etiqueta->setFont(QFont("Arial", 10, QFont::Bold));
                etiqueta->setDefaultTextColor(Qt::white);
                QRectF textRect = etiqueta->boundingRect();

                // Fondo
                QGraphicsRectItem* fondo = new QGraphicsRectItem(textRect.adjusted(-6, -4, 6, 4));
                fondo->setBrush(Qt::black);
                fondo->setPen(QPen(serie->color(), 1));
                fondo->setZValue(0);

                // Posición base
                QPointF puntoGraf = chartAllView->chart()->mapToPosition(punto, serie);
                QPointF posEtiqueta = puntoGraf + QPointF(10, -textRect.height() - 6);

                QRectF nuevaCaja(posEtiqueta, fondo->rect().size());

                // Desplazar si se superpone
                for (const QRectF& ocupado : ocupados) {
                    while (nuevaCaja.intersects(ocupado)) {
                        posEtiqueta.ry() -= 20;
                        nuevaCaja.moveTopLeft(posEtiqueta);
                    }
                }
                ocupados.append(nuevaCaja);

                fondo->setPos(posEtiqueta);
                etiqueta->setPos(posEtiqueta);

                // Línea de unión
                QPointF centroEtiqueta = posEtiqueta + QPointF(fondo->rect().width() / 2, fondo->rect().height() / 2);
                QGraphicsLineItem* linea = new QGraphicsLineItem(QLineF(puntoGraf, centroEtiqueta));
                linea->setPen(QPen(serie->color(), 1, Qt::DashLine));
                linea->setZValue(-1);

                // Añadir a escena
                chartAllView->scene()->addItem(fondo);
                chartAllView->scene()->addItem(etiqueta);
                chartAllView->scene()->addItem(linea);

                // Guardar para limpiar luego
                tooltipTexts.append(fondo);
                tooltipTexts.append(etiqueta);
                tooltipLines.append(linea);
            }
        }

        // === Gráfico 3D ===
        QVector3D pos(d.longitude, xIndex3D, d.latitude);
        pointHistory.append(pos);

        const int N = 2;
        int startTrail = std::max(0, int(pointHistory.size() - N));

        QScatterDataArray* trailArray = new QScatterDataArray();
        trailArray->resize(pointHistory.size() - startTrail);
        for (int i = startTrail; i < pointHistory.size(); ++i) {
            float t = float(i - startTrail) / float(N - 1);
            QColor c; c.setRgbF(t, 0.0, 1.0 - t);
            QScatterDataItem item;
            item.setPosition(pointHistory[i]);
            trailSeries->setBaseColor(c);
            (*trailArray)[i - startTrail] = item;
        }
        trailSeries->dataProxy()->resetArray(trailArray);

        xIndex3D++;

        // === LLMAP ===

        double x = d.longitude * 10000;
        double y = -d.latitude * 10000;

        if (mapDot) llmapScene->removeItem(mapDot);
        mapDot = llmapScene->addEllipse(x - 5, y - 5, 10, 10, QPen(Qt::red), QBrush(Qt::red));
        llmap->centerOn(mapDot);

        trayecto.lineTo(x, y);
        pathItem->setPath(trayecto);

        // === Gyro3D ===
        QQuaternion qYaw = QQuaternion::fromEulerAngles(0.0f, d.Yaw, 0.0f);
        QQuaternion qPitch = QQuaternion::fromEulerAngles(d.Pitch, 0.0f, 0.0f);
        QQuaternion qRoll = QQuaternion::fromEulerAngles(0.0f, 0.0f, d.Roll);
        QQuaternion orientation = qYaw * qPitch * qRoll;

        rocketTransform->setRotation(orientation);

        QTimer::singleShot(1500, [=]() {
            if (mesh->status() == Qt3DRender::QMesh::Ready) {
                qDebug() << "✅ Modelo STL cargado correctamente.";

                // Ajustamos la cámara para ver todo el modelo
                auto bbCenter = rocketTransform->translation();  // Centro de transformación
                float distance = 2.5f; // Aumenta o reduce según lo que veas
                camera->setViewCenter(bbCenter);
                camera->setPosition(bbCenter + QVector3D(0, 0, distance));

            } else {
                qDebug() << "❌ Error o el modelo aún no está listo.";
            }
        });
    });

}
