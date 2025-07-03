#include "widget.h"
#include "Graph3DWindow.h"
#include "data/FileHelper.h"

#include <QMenu>
#include <QWidgetAction>
#include <QPushButton>
#include <QMessageBox>

#include <QGridLayout>
#include <QLabel>
#include <QChartView>
#include <QDateTime>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>

static Widget* ventanaUnica = nullptr;
Graph3DWindow* ventanaGraph3D = nullptr;

Widget::Widget(SensorManager* manager, QWidget* parent)
    : QWidget(parent), m_sensorManager(manager) {
    
    // === Verificación de instancia única ===
    if (ventanaUnica && ventanaUnica != this) {
        close();
        return;
    }
    ventanaUnica = this;

    for (int i = 0; i < 6; ++i) labelStatus[i] = nullptr;
    for (int i = 0; i < 4; ++i) labelServos[i] = nullptr;


    setWindowTitle("FRECCIA_XAE - Gráficas 2D");
    setStyleSheet("background-color: black;");
    QGridLayout* layout = new QGridLayout();
    layout->setSpacing(2);
    setWindowIcon(QIcon("./assets/logo_xae.png"));

    pantalla1Activa = true;
    actualizarEstilosMenu();

    // === Menu ===
    QWidget* topBar = new QWidget();
    topBar->setStyleSheet("background-color: black; color: white;");
    topBar->setFixedHeight(30);

    QHBoxLayout* topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(5, 0, 5, 0);

    // === Botones ===
    QPushButton* btnRecord = new QPushButton("● Grabar");
    btnRecord->setStyleSheet(
        "QPushButton { color: white; background-color: transparent; border: none; }"
        "QPushButton:hover { background-color: #444; }"
    );

    QPushButton* btnStop = new QPushButton("■ Detener");
    btnStop->setStyleSheet(
        "QPushButton { color: white; background-color: transparent; border: none; }"
        "QPushButton:hover { background-color: #444; }"
    );

    QPushButton* btnVerAntiguos = new QPushButton("Ver antiguos");
    btnVerAntiguos->setStyleSheet(
        "QPushButton { color: white; background-color: transparent; border: none; }"
        "QPushButton:hover { background-color: #444; }"
    );

    // FileHelper inicializado
    fileHelper = new FileHelper();

    // === Conexión botón GRABAR ===
    connect(btnRecord, &QPushButton::clicked, this, [this, btnRecord]() {
        fileHelper->iniciarGrabacion();
        btnRecord->setStyleSheet(
            "QPushButton { color: red; background-color: transparent; border: none; }"
            "QPushButton:hover { background-color: #444; }"
        );
    });

    // === Conexión botón DETENER ===
    connect(btnStop, &QPushButton::clicked, this, [this, btnStop, btnRecord]() {
        fileHelper->detenerGrabacion();

        // Cambia a rojo por 1.2 segundos
        btnStop->setStyleSheet(
            "QPushButton { color: red; background-color: transparent; border: none; }"
            "QPushButton:hover { background-color: #444; }"
        );

        QTimer::singleShot(1200, this, [btnStop]() {
            btnStop->setStyleSheet(
                "QPushButton { color: white; background-color: transparent; border: none; }"
                "QPushButton:hover { background-color: #444; }"
            );
        });

        // Restaurar estilo de btnRecord
        btnRecord->setStyleSheet(
            "QPushButton { color: white; background-color: transparent; border: none; }"
            "QPushButton:hover { background-color: #444; }"
        );
    });


    QWidget* leftButtons = new QWidget();
    QHBoxLayout* leftLayout = new QHBoxLayout(leftButtons);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->addWidget(btnRecord);
    leftLayout->addWidget(btnStop);
    leftLayout->addWidget(btnVerAntiguos);

    // === Tiempo ===
    labelTiempo = new QLabel("Tiempo: 00:00:00");
    labelTiempo->setAlignment(Qt::AlignCenter);
    labelTiempo->setStyleSheet("color: white; font-weight: bold;");

    // Menú desplegable y acciones
   QPushButton* btnMenu = new QPushButton();
    btnMenu->setIcon(QIcon("./assets/Menu.png"));
    btnMenu->setIconSize(QSize(35, 35));
    btnMenu->setStyleSheet("background-color: transparent; border: none;");

    // Menú
    QMenu* menuDesplegable = new QMenu(this);
    menuDesplegable->setStyleSheet("QMenu { background-color: black; color: white; }"
                                "QMenu::item:selected { background-color: #444; }");

    btnMenu->setMenu(menuDesplegable);
    // Acciones
    pantalla1 = menuDesplegable->addAction("Pantalla Gráficas 2D");
    pantalla2 = menuDesplegable->addAction("Pantalla Gráficas 3D y OSM");

    // Estilo para el menú completo
    menuDesplegable->setStyleSheet(R"(
        QMenu {
            background-color: black;
            color: white;
        }
        QMenu::item {
            padding: 6px 24px;
            background-color: black;
            color: white;
        }
        QMenu::item:disabled {
            background-color: green;
            color: white;
        }
        QMenu::item:selected:enabled {
            background-color: #444;
        }
    )");

    // === Detectar si ya está abierta ===
    if (ventanaUnica && ventanaUnica != this) {
            close();
            return;
        }
        ventanaUnica = this;
        pantalla1->setEnabled(false);
        QObject::connect(this, &QWidget::destroyed, []() {
            ventanaUnica = nullptr;
    });

    // === Boton Pantalla Gráficas 2D ===
    connect(pantalla1, &QAction::triggered, this, [manager]() {
        if (!ventanaUnica) {
            ventanaUnica = new Widget(manager);
            ventanaUnica->resize(1280, 720);
            ventanaUnica->show();
        } else {
            ventanaUnica->raise();
            ventanaUnica->activateWindow();
        }
    });

    if (ventanaGraph3D) {
        pantalla2->setEnabled(false);
        pantalla1Activa = false;
        actualizarEstilosMenu();
    }
    // === Botón Pantalla Gráficas 3D y OSM ===
    connect(pantalla2, &QAction::triggered, this, [this, manager]() {
        if (ventanaGraph3D) {
            ventanaGraph3D->raise();
            ventanaGraph3D->activateWindow();
            return;
        }

        pantalla1Activa = false;
        actualizarEstilosMenu();

        pantalla2->setEnabled(false);

        ventanaGraph3D = new Graph3DWindow(manager);
        ventanaGraph3D->setAttribute(Qt::WA_DeleteOnClose);
        ventanaGraph3D->resize(1280, 720);
        ventanaGraph3D->show();

        connect(ventanaGraph3D, &QWidget::destroyed, this, [this]() {
            pantalla2->setEnabled(true);
            actualizarEstilosMenu();
            ventanaGraph3D = nullptr;
        });
    });

    menuDesplegable->addSeparator();
    QAction* configCom = menuDesplegable->addAction("Configurar COM");
    QAction* configBaud = menuDesplegable->addAction("Configurar Baud");
    QAction* configAcc = menuDesplegable->addAction("Calibrar Acelerómetro");
    QAction* configGyro = menuDesplegable->addAction("Calibrar Giroscopio");
    menuDesplegable->addSeparator();
    QWidgetAction* cerrarWidgetAction = new QWidgetAction(this);
    QPushButton* cerrarBtn = new QPushButton("Close the program");
    cerrarBtn->setStyleSheet("color: white; background-color: red; border: none; padding: 4px;");
    cerrarWidgetAction->setDefaultWidget(cerrarBtn);
    menuDesplegable->addAction(cerrarWidgetAction);

    btnMenu->setMenu(menuDesplegable);

    // === Añadir a layout superior ===
    topLayout->addWidget(leftButtons);
    topLayout->addStretch();
    topLayout->addWidget(labelTiempo);
    topLayout->addStretch();
    topLayout->addWidget(btnMenu);

    // === Añadir barra al layout principal ===
    QVBoxLayout* globalLayout = new QVBoxLayout(this);
    globalLayout->setContentsMargins(0, 0, 0, 0);
    globalLayout->addWidget(topBar);
    globalLayout->addLayout(layout);

    auto crearGrafica = [&](QChart*& chart, QLineSeries*& series, QChartView*& view,
                        QLabel*& label, QValueAxis*& ejeX, QValueAxis*& ejeY,
                        const QString& nombre, const QString& yTitulo,
                        int fila, int columna) {
        chart = new QChart();
        series = new QLineSeries();
        series->setName(nombre);

        // === Colores únicos por gráfica ===
        static QVector<QColor> colores = {
            Qt::cyan, Qt::magenta, Qt::green, Qt::yellow,
            Qt::red, Qt::blue, Qt::gray, Qt::darkCyan,
            Qt::darkMagenta, Qt::darkYellow, Qt::darkRed, Qt::darkBlue
        };
        static int colorIndex = 0;
        series->setColor(colores[colorIndex % colores.size()]);
        colorIndex++;

        chart->addSeries(series);

        // === Ejes inicializados desde 0 ===
        ejeX = new QValueAxis();
        ejeY = new QValueAxis();
        ejeX->setTitleText("Tiempo");
        ejeY->setTitleText(yTitulo);
        ejeX->setLabelsColor(Qt::white);
        ejeY->setLabelsColor(Qt::white);
        ejeX->setTitleBrush(QBrush(Qt::white));
        ejeY->setTitleBrush(QBrush(Qt::white));
        ejeX->setRange(0, 1);
        ejeY->setRange(0, 1); //No menor a 12

        chart->addAxis(ejeX, Qt::AlignBottom);
        chart->addAxis(ejeY, Qt::AlignLeft);
        series->attachAxis(ejeX);
        series->attachAxis(ejeY);

        chart->setTitle(nombre);
        chart->setTitleBrush(QBrush(Qt::white));
        chart->legend()->setLabelColor(Qt::white);
        chart->setBackgroundBrush(QBrush(Qt::black));

        view = new QChartView(chart);
        view->setMinimumSize(300, 200);
        layout->addWidget(view, fila, columna);

        label = new QLabel(nombre + ": 0");
        label->setStyleSheet("color: white; font-weight: bold;");
        layout->addWidget(label, fila + 1, columna);
    };

    // Fila 0
    crearGrafica(chartRoll, seriesRoll, viewRoll, labelRoll, axisX_Roll, axisY_Roll, "Roll", "Ángulo", 0, 0);
    crearGrafica(chartPitch, seriesPitch, viewPitch, labelPitch, axisX_Pitch, axisY_Pitch, "Pitch", "Ángulo", 0, 1);
    crearGrafica(chartYaw, seriesYaw, viewYaw, labelYaw, axisX_Yaw, axisY_Yaw, "Yaw", "Ángulo", 0, 2);
    crearGrafica(chartSats, seriesSats, viewSats, labelSats, axisX_Sats, axisY_Sats, "Satélites", "Número", 0, 3);

    // Fila 1
    crearGrafica(chartLat, seriesLat, viewLat, labelLat, axisX_Lat, axisY_Lat, "Latitud", "Grados", 1, 0);
    crearGrafica(chartLon, seriesLon, viewLon, labelLon, axisX_Lon, axisY_Lon, "Longitud", "Grados", 1, 1);
    crearGrafica(chartAlt, seriesAlt, viewAlt, labelAlt, axisX_Alt, axisY_Alt, "AltDiff", "m", 1, 2);
    crearGrafica(chartHdop, seriesHdop, viewHdop, labelHdop, axisX_Hdop, axisY_Hdop, "HDOP", "Precisión", 1, 3);

    // Fila 2
    crearGrafica(chartPresion, seriesPressure, viewPressure, labelPressure, axisX_Pressure, axisY_Pressure, "Presión", "hPa", 2, 0);
    crearGrafica(chartTemp, seriesTemp, viewTemp, labelTemp, axisX_Temp, axisY_Temp, "Temperatura", "°C", 2, 1);

   // === SERVOS (2,2) como tarjetas cuadradas ===
    QGridLayout* gridServos = new QGridLayout();
    gridServos->setSpacing(10);

    QStringList unidadesServo = {"°", "°", "°", "°"};

    for (int i = 0; i < 4; ++i) {
        QFrame* card = new QFrame();
        card->setFrameShape(QFrame::NoFrame); // Quita el borde
        card->setStyleSheet("background-color: #2c2c2c; border-radius: 10px;");
        card->setFixedSize(100, 100);

        QVBoxLayout* cardLayout = new QVBoxLayout(card);
        cardLayout->setAlignment(Qt::AlignCenter);
        cardLayout->setSpacing(4);
        cardLayout->setContentsMargins(4, 4, 4, 4);

        QLabel* labelTitulo = new QLabel(QString("Servo %1").arg(i + 1));
        QFont tituloFont;
        tituloFont.setPointSize(8);
        tituloFont.setBold(true);
        labelTitulo->setFont(tituloFont);
        labelTitulo->setStyleSheet("color: white;");
        labelTitulo->setAlignment(Qt::AlignCenter);

        labelServos[i] = new QLabel("0" + unidadesServo[i]);
        QFont valorFont;
        valorFont.setPointSize(10);
        valorFont.setBold(true);
        labelServos[i]->setFont(valorFont);
        labelServos[i]->setStyleSheet("color: white;");
        labelServos[i]->setAlignment(Qt::AlignCenter);

        cardLayout->addWidget(labelTitulo);
        cardLayout->addWidget(labelServos[i]);

        gridServos->addWidget(card, i / 2, i % 2); // Posición 2x2
    }

    QWidget* servoWidget = new QWidget();
    servoWidget->setLayout(gridServos);
    layout->addWidget(servoWidget, 2, 2);


    // === ESTADO DEL SISTEMA (2,3) con distribución 3 filas x 2 columnas ===
    QGridLayout* gridEstado = new QGridLayout();
    gridEstado->setSpacing(8);
    gridEstado->setContentsMargins(0, 0, 0, 0);

    QStringList campos = {"Conexión", "Tiempo Para Inicio", "Paracaídas", "Fecha Satélital", "Fecha Local", "Hora Local"};

    for (int i = 0; i < campos.size(); ++i) {
    QFrame* card = new QFrame();
    if (!card) {
        qDebug() << "Error: card es nullptr en índice" << i;
        continue;
    }

    card->setStyleSheet("background-color: #2c2c2c; border-radius: 10px;");
    card->setFixedSize(150, 50);

    QVBoxLayout* cardLayout = new QVBoxLayout(card);
    cardLayout->setAlignment(Qt::AlignCenter);
    cardLayout->setContentsMargins(4, 2, 4, 2);

    QLabel* titulo = new QLabel(campos[i]);
    titulo->setStyleSheet("color: white; font-size: 10px;");
    titulo->setAlignment(Qt::AlignCenter);

    labelStatus[i] = new QLabel("Esperando...");
    labelStatus[i]->setStyleSheet("color: white; font-weight: bold; font-size: 12px;");
    labelStatus[i]->setAlignment(Qt::AlignCenter);

    cardLayout->addWidget(titulo);
    cardLayout->addWidget(labelStatus[i]);

    if (labelStatus[i] == nullptr || titulo == nullptr || cardLayout == nullptr) {
        qDebug() << "Fallo en creación de widgets para campo" << campos[i];
        continue;
    }

    gridEstado->addWidget(card, i / 2, i % 2); // <--- aquí crashea si card == nullptr
    }

    // === Línea inferior con COM y Velocidad ===
    QHBoxLayout* puertoLayout = new QHBoxLayout();
    puertoLayout->setSpacing(12);
    puertoLayout->setContentsMargins(0, 0, 0, 0);

    labelCom = new QLabel("COM: Esperando...");
    labelCom->setStyleSheet("color: white; font-size: 11px; font-weight: bold;");
    labelCom->setAlignment(Qt::AlignLeft);

    labelBaud = new QLabel("Velocidad: Esperando...");
    labelBaud->setStyleSheet("color: white; font-size: 11px; font-weight: bold;");
    labelBaud->setAlignment(Qt::AlignRight);

    puertoLayout->addWidget(labelCom);
    puertoLayout->addStretch(); // Para que se separen bien
    puertoLayout->addWidget(labelBaud);

    // === Línea final con paquete RAW ===
    labelRaw = new QLabel("Paquete: Esperando...");
    labelRaw->setStyleSheet("color: white; font-size: 10px; background-color: #1e1e1e; padding: 6px;");
    labelRaw->setWordWrap(true);
    labelRaw->setAlignment(Qt::AlignLeft);
    labelRaw->setMinimumHeight(30);

    QVBoxLayout* estadoFinal = new QVBoxLayout();
    estadoFinal->addLayout(gridEstado);

    qDebug() << "Estado de labelRaw:" << (labelRaw ? "OK" : "nullptr");

    if (labelRaw) {
        estadoFinal->addWidget(labelRaw);
    } else {
        QLabel* fallbackRaw = new QLabel("Error: RAW no inicializado");
        fallbackRaw->setStyleSheet("color: red;");
        estadoFinal->addWidget(fallbackRaw);
    }

    QWidget* estadoWidget = new QWidget();
    estadoWidget->setLayout(estadoFinal);
    layout->addWidget(estadoWidget, 2, 3);
    qDebug() << "estadoWidget agregado al layout";

    // === Datos en tiempo real ===
    connect(m_sensorManager, &SensorManager::newSensorData, this, [&](const SensorData& d) {
        static int t = 0;

        qDebug() << "Recibiendo datos del sensor...";
        if (!labelRaw) qWarning() << "labelRaw es nullptr";
        for (int i = 0; i < 4; ++i)
            if (!labelServos[i]) qWarning() << "labelServos[" << i << "] es nullptr";
        for (int i = 0; i < 6; ++i)
            if (!labelStatus[i]) qWarning() << "labelStatus[" << i << "] es nullptr";
        if (!seriesRoll || !labelRoll || !axisX_Roll || !axisY_Roll) qWarning() << "Falta algo en Roll";

        auto actualizarGrafica = [&](QLineSeries* series, double valor, QLabel* label, QValueAxis* axisX, QValueAxis* axisY) {
            if (!series || !label || !axisX || !axisY || std::isnan(valor) || std::isinf(valor)) return;
            series->append(t, valor);
            label->setText(QString("%1: %2").arg(series->name()).arg(valor));
            if (axisX->min() == axisX->max()) axisX->setRange(t, t + 1);
            else { if (t > axisX->max()) axisX->setMax(t); if (t < axisX->min()) axisX->setMin(t); }
            if (axisY->min() == axisY->max()) axisY->setRange(valor, valor + 1);
            else { if (valor > axisY->max()) axisY->setMax(valor); if (valor < axisY->min()) axisY->setMin(valor); }
        };

        actualizarGrafica(seriesRoll, d.Roll, labelRoll, axisX_Roll, axisY_Roll);
        actualizarGrafica(seriesPitch, d.Pitch, labelPitch, axisX_Pitch, axisY_Pitch);
        actualizarGrafica(seriesYaw, d.Yaw, labelYaw, axisX_Yaw, axisY_Yaw);
        actualizarGrafica(seriesSats, d.satellites, labelSats, axisX_Sats, axisY_Sats);
        actualizarGrafica(seriesLat, d.latitude, labelLat, axisX_Lat, axisY_Lat);
        actualizarGrafica(seriesLon, d.longitude, labelLon, axisX_Lon, axisY_Lon);
        actualizarGrafica(seriesAlt, d.AltDiff, labelAlt, axisX_Alt, axisY_Alt);
        actualizarGrafica(seriesHdop, d.hdop, labelHdop, axisX_Hdop, axisY_Hdop);
        actualizarGrafica(seriesPressure, d.pressure, labelPressure, axisX_Pressure, axisY_Pressure);
        actualizarGrafica(seriesTemp, d.temperature, labelTemp, axisX_Temp, axisY_Temp);

        labelServos[0]->setText(QString::number(d.Servo1) + "°");
        labelServos[1]->setText(QString::number(d.Servo2) + "°");
        labelServos[2]->setText(QString::number(d.Servo3) + "°");
        labelServos[3]->setText(QString::number(d.Servo4) + "°");

        labelStatus[0]->setText("Estable - 3");
        labelStatus[1]->setText("Inicializado");
        labelStatus[2]->setText("N/A");
        labelStatus[3]->setText(QString::fromStdString(d.date));
        labelStatus[4]->setText(QDate::currentDate().toString("dd-MMMM-yyyy"));
        labelStatus[5]->setText(QTime::currentTime().toString("hh:mm:ss"));

        labelRaw->setText("Paquete: " +
            QString::number(d.latitude) + "," +
            QString::number(d.longitude) + "," +
            QString::fromStdString(d.date) + "," +
            QString::fromStdString(d.utc_time) + "," +
            QString::number(d.secs) + "," +
            QString::number(d.satellites) + "," +
            QString::number(d.hdop) + "," +
            QString::number(d.Roll) + "," +
            QString::number(d.Pitch) + "," +
            QString::number(d.Yaw) + "," +
            QString::number(d.Servo1) + "," +
            QString::number(d.Servo2) + "," +
            QString::number(d.Servo3) + "," +
            QString::number(d.Servo4) + "," +
            QString::number(d.AltDiff)
        );

       timeoutTimer->start();

        if (!tiempoIniciado) {
            tiempoInicio = QTime::currentTime();
            tiempoIniciado = true;
            timer->start(1000);
        } else if (!timer->isActive()) {
            timer->start(1000);
            qDebug() << "Señal recuperada. Reanudando cronómetro.";
        }

        ++t;

        procesarDatos(d);
    });
    
    this->timer = new QTimer(this);
    timeoutTimer = new QTimer(this);
    timeoutTimer->setInterval(30000);
    timeoutTimer->setSingleShot(true);

    connect(timer, &QTimer::timeout, this, [this]() {
            int secs = tiempoInicio.secsTo(QTime::currentTime());
            QTime t(0, 0);
            t = t.addSecs(secs);
            labelTiempo->setText("Tiempo: " + t.toString("hh:mm:ss"));
        });
    
    connect(timeoutTimer, &QTimer::timeout, this, [this]() {
        qWarning() << "No se han recibido datos en 20 segundos. Deteniendo el contador.";
        if (timer->isActive()) {
            timer->stop();
            labelTiempo->setText("Señal perdida. Cronómetro detenido.");
        }
    });

    connect(cerrarBtn, &QPushButton::clicked, this, []() {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle("Exit Confirmation");
            msgBox.setText("Are you sure about closing the program?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);

            msgBox.setWindowIcon(QIcon("./assets/logo_xae.png"));

            int reply = msgBox.exec();
            if (reply == QMessageBox::Yes)
                QCoreApplication::quit();
        });
    pantalla1Activa = true;
    actualizarEstilosMenu();
}

void Widget::actualizarEstilosMenu() {
    if (pantalla1 && pantalla2) {
        pantalla1->setIcon(pantalla1Activa ? QIcon(":/icons/activo.png") : QIcon());
    pantalla2->setIcon(!pantalla1Activa ? QIcon(":/icons/activo.png") : QIcon());
    }
}

void Widget::abrirVentana3DDesdeExterno() {
    if (ventanaGraph3D) return;
    pantalla2->setEnabled(false);
    actualizarEstilosMenu();

    connect(ventanaGraph3D, &QWidget::destroyed, this, [this]() {
        pantalla2->setEnabled(true);
        actualizarEstilosMenu();
        ventanaGraph3D = nullptr;
    });
}

void Widget::procesarDatos(const SensorData& data) {

    fileHelper->escribirDuranteGrabacion(data);
}

Widget::~Widget() {}
