#ifndef INICIOWINDOW_H
#define INICIOWINDOW_H

#include <QWidget>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

class InicioWindow : public QWidget {
    Q_OBJECT

public:
    explicit InicioWindow(QWidget* parent = nullptr);

signals:
    void iniciar(bool abrir2D, bool abrir3D);

private:
    QCheckBox* check2D;
    QCheckBox* check3D;
    QPushButton* btnIniciar;
    QPushButton* btnExit;
    QLabel* gifLabel;

    void setupStyle();
    void checkSelection();
};

#endif // INICIOWINDOW_H
