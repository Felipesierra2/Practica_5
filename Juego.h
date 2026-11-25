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
#include "HUD.h"

class Juego : public QGraphicsView
{
    Q_OBJECT

public:
    explicit Juego(QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* evento) override;

private:
    static constexpr qreal FACTOR_DANO = 0.05;
    static constexpr qreal FACTOR_DANO_ESTRUCTURAL = 1.0;

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

    HUD* hud = nullptr;

    void crearHUD();
    void comprobarFinPartida();
    void finalizarPartida();
    void inicializarEscenario();
    void dispararParticula();
    void actualizarJuego();
    void eliminarParticulasFueraEscenario();
    void comprobarColisionesParedes();
    void cambiarTurno();
    void crearIndicadores();
    void actualizarIndicadores();
    void crearParedes();
    void crearObstaculos();
    void comprobarColisionesObstaculos();
    void reiniciarPartida();
    void actualizarIndicadorTurno();

    QGraphicsLineItem* indicadorLanzamiento;

    Jugador* jugador1;
    Jugador* jugador2;
    Jugador* jugadorActual;

    void crearJugadores();
    void comprobarColisionesJugadores();
};

#endif // JUEGO_H
