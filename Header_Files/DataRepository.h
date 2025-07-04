#ifndef DATAREPOSITORY_H
#define DATAREPOSITORY_H

#include <vector>
#include <QString>
#include <QObject>
#include "SensorData.h"

class DataRepository : public QObject {
    Q_OBJECT

public:
    enum class Modo { NINGUNO, EN_VIVO, HISTORICO };

    static DataRepository* instance();  // Singleton con puntero

    void agregarDatoEnVivo(const SensorData& d);   // para SensorManager
    bool cargarDesdeArchivo(const QString& ruta);  // para histórico

    const std::vector<SensorData>& obtenerDatos() const;
    Modo obtenerModo() const;
    void reiniciar();

signals:
    void newData(const SensorData& d);

private:
    explicit DataRepository(QObject* parent = nullptr);

    std::vector<SensorData> datos;
    Modo modoActual = Modo::NINGUNO;
};

#endif // DATAREPOSITORY_H
