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
#include "registrosimulacion.h"
#include "Jugador.h"

class Juego : public QGraphicsView
{
    Q_OBJECT

public:
    explicit Juego(QWidget* parent = nullptr);

    void crearIndicadores();
    void actualizarIndicadores();
    void crearParedes();
    void crearObstaculos();
    void comprobarColisionesObstaculos();
    void comprobarColisionesParticulas();

protected:
    void keyPressEvent(QKeyEvent* evento) override;

private:
    static constexpr qreal FACTOR_DANO = 0.05;

    QGraphicsScene* escena;
    QTimer* temporizador;
    QElapsedTimer reloj;

    QList<Particula*> particulas;
    QList<Pared*> paredes;
    QList<Obstaculo*> obstaculos;

    Particula* particulaActual;

    QPointF posicionLanzamiento;

    RegistroSimulacion registro;
    qreal tiempoSimulacion;
    qreal anguloLanzamiento;
    qreal rapidezLanzamiento;

    bool partidaFinalizada;

    void comprobarFinPartida();
    void finalizarPartida();
    void inicializarEscenario();
    void crearParticula();
    void dispararParticula();
    void actualizarJuego();
    void eliminarParticulasFueraEscenario();
    void comprobarColisionesParedes();
    void fusionarParticulas(int indice1, int indice2);
    void registrarPosiciones();
    void cambiarTurno();

    QGraphicsLineItem* indicadorLanzamiento;
    QGraphicsTextItem* informacionLanzamiento;

    Jugador* jugador1;
    Jugador* jugador2;
    Jugador* jugadorActual;

    void crearJugadores();
    void comprobarColisionesJugadores();
};

#endif // JUEGO_H
