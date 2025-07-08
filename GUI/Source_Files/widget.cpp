#include "widget.h"
#include "Graph3DWindow.h"
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

Widget::Widget(SensorManager* manager, QWidget* parent)
    : QWidget(parent), m_sensorManager(manager) {
    
    for (int i = 0; i < 6; ++i) labelStatus[i] = nullptr;
    for (int i = 0; i < 4; ++i) labelServos[i] = nullptr;


    setWindowTitle("GUI_FRECCIA_Dashboard");
    setStyleSheet("background-color: black;");
    QGridLayout* layout = new QGridLayout(this);
    layout->setSpacing(2);
    layout->setContentsMargins(0, 0, 0, 0);

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

    QStringList campos = {"Conexión", "Inicio", "Paracaídas", "Fecha UTC", "Fecha PC", "Hora"};

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

        labelStatus[0]->setText("Conexión: OK");
        labelStatus[1]->setText("Inicio: Recibido");
        labelStatus[2]->setText("Paracaídas: N/A");
        labelStatus[3]->setText("Fecha UTC: " + QString::fromStdString(d.date));
        labelStatus[4]->setText("Fecha PC: " + QDate::currentDate().toString("yyyy-MM-dd"));
        labelStatus[5]->setText("Hora: " + QTime::currentTime().toString("hh:mm:ss"));

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

        ++t;
    });
}

Widget::~Widget() {}
