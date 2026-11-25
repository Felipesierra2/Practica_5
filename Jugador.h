#ifndef JUGADOR_H
#define JUGADOR_H

#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QVector2D>

#include "Particula.h"


class Jugador : public QGraphicsItemGroup {
private:
    static constexpr qreal VIDA_MAXIMA = 100.0;

    qreal vida;

    QGraphicsRectItem* fondoVida;
    QGraphicsRectItem* barraVida;

    static constexpr qreal ANCHO_BARRA_VIDA = 50.0;
    static constexpr qreal ALTO_BARRA_VIDA = 6.0;

    void crearBarraVida();
    void actualizarBarraVida();

    static constexpr qreal ANGULO_MINIMO = 0.0;
    static constexpr qreal ANGULO_MAXIMO = 90.0;

    static constexpr qreal RAPIDEZ_MINIMA = 1.0;
    static constexpr qreal RAPIDEZ_MAXIMA = 40.0;

    qreal anguloLanzamiento;
    qreal rapidezLanzamiento;

    bool mirandoDerecha;

    QGraphicsLineItem* canon;
    QGraphicsEllipseItem* base;

    void actualizarCanon();

public:
    explicit Jugador(bool mirandoDerecha,
                     QGraphicsItem* parent = nullptr);

    void modificarAngulo(qreal incremento);
    void modificarRapidez(qreal incremento);
    void recibirDano(qreal cantidad);

    qreal obtenerVida() const;
    qreal obtenerAngulo() const;
    qreal obtenerRapidez() const;

    bool estaDerrotado() const;
    bool detectarImpacto(const Particula* particula) const;

    QPointF obtenerPosicionLanzamiento() const;
    QVector2D obtenerVelocidadLanzamiento() const;


};

#endif // JUGADOR_H
