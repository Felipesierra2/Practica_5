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
#include "pared.h"
#include "obstaculo.h"
#include "Jugador.h"

class Juego : public QGraphicsView
{
    Q_OBJECT

public:
    explicit Juego(QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* evento) override;

private:
    static constexpr qreal FACTOR_DANO = 0.05;

    QGraphicsTextItem* mensajeResultado = nullptr;
    QGraphicsScene* escena;
    QTimer* temporizador;
    QElapsedTimer reloj;

    QList<Particula*> particulas;
    QList<Pared*> paredes;
    QList<Obstaculo*> obstaculos;

    Particula* crearParticula();

    QPointF posicionLanzamiento;

    qreal anguloLanzamiento;

    bool partidaFinalizada;

    void comprobarFinPartida();
    void finalizarPartida();
    void inicializarEscenario();
    void dispararParticula();
    void actualizarJuego();
    void eliminarParticulasFueraEscenario();
    void comprobarColisionesParedes();
    void fusionarParticulas(int indice1, int indice2);
    void cambiarTurno();
    void crearIndicadores();
    void actualizarIndicadores();
    void crearParedes();
    void crearObstaculos();
    void comprobarColisionesObstaculos();
    void comprobarColisionesParticulas();
    void reiniciarPartida();
    void actualizarIndicadorTurno();

    QGraphicsLineItem* indicadorLanzamiento;
    QGraphicsTextItem* informacionLanzamiento;

    Jugador* jugador1;
    Jugador* jugador2;
    Jugador* jugadorActual;

    void crearJugadores();
    void comprobarColisionesJugadores();
};

#endif // JUEGO_H
