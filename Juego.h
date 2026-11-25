#ifndef JUEGO_H
#define JUEGO_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <QElapsedTimer>
#include <QKeyEvent>
#include <QList>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>

#include "particula.h"

class Juego : public QGraphicsView
{
    Q_OBJECT

public:
    explicit Juego(QWidget* parent = nullptr);

    void crearIndicadores();
    void actualizarIndicadores();

protected:
    void keyPressEvent(QKeyEvent* evento) override;

private:
    QGraphicsScene* escena;
    QTimer* temporizador;
    QElapsedTimer reloj;

    QList<Particula*> particulas;

    Particula* particulaActual;

    QPointF posicionLanzamiento;

    qreal anguloLanzamiento;
    qreal rapidezLanzamiento;

    void inicializarEscenario();
    void crearParticula();
    void dispararParticula();
    void actualizarJuego();
    void eliminarParticulasFueraEscenario();

    QGraphicsLineItem* indicadorLanzamiento;
    QGraphicsTextItem* informacionLanzamiento;
};

#endif // JUEGO_H
