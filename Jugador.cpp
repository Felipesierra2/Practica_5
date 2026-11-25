#include "Jugador.h"
#include <QPen>
#include <QBrush>
#include <QtMath>

Jugador::Jugador(bool mirandoDerecha, QGraphicsItem* parent)
    : QGraphicsItemGroup(parent),
    anguloLanzamiento(45.0),
    rapidezLanzamiento(20.0),
    mirandoDerecha(mirandoDerecha),
    vida(VIDA_MAXIMA)
{
    base = new QGraphicsEllipseItem(-20, -20, 40, 40);

    base->setBrush(Qt::darkGray);
    base->setPen(QPen(Qt::black, 2));

    canon = new QGraphicsLineItem(0, 0, 35, 0);

    canon->setPen(QPen(Qt::black, 8));

    addToGroup(base);
    addToGroup(canon);

    actualizarCanon();
    crearBarraVida();
}

void Jugador::actualizarCanon()
{
    const qreal longitudCanon = 35.0;

    qreal anguloRadianes = qDegreesToRadians(anguloLanzamiento);

    qreal direccion = mirandoDerecha ? 1.0 : -1.0;

    qreal x = direccion * longitudCanon * qCos(anguloRadianes);
    qreal y = -longitudCanon * qSin(anguloRadianes);

    canon->setLine(0, 0, x, y);
}

void Jugador::modificarAngulo(qreal incremento)
{
    anguloLanzamiento = qBound(
        ANGULO_MINIMO,
        anguloLanzamiento + incremento,
        ANGULO_MAXIMO
        );

    actualizarCanon();
}

void Jugador::modificarRapidez(qreal incremento)
{
    rapidezLanzamiento = qBound(
        RAPIDEZ_MINIMA,
        rapidezLanzamiento + incremento,
        RAPIDEZ_MAXIMA
        );
}

void Jugador::recibirDano(qreal cantidad)
{
    if (cantidad <= 0 || estaDerrotado())
        return;

    vida = qMax(0.0, vida - cantidad);

    actualizarBarraVida();
}

void Jugador::reiniciar()
{
    vida = VIDA_MAXIMA;

    anguloLanzamiento = 45.0;
    rapidezLanzamiento = 20.0;

    actualizarCanon();
    actualizarBarraVida();
}

qreal Jugador::obtenerVida() const
{
    return vida;
}

bool Jugador::estaDerrotado() const
{
    return vida <= 0.0;
}

bool Jugador::detectarImpacto(const Particula* particula) const
{
    if (!particula || estaDerrotado())
        return false;

    return base->collidesWithItem(
        particula,
        Qt::IntersectsItemShape
        );
}

qreal Jugador::obtenerAngulo() const
{
    return anguloLanzamiento;
}

qreal Jugador::obtenerRapidez() const
{
    return rapidezLanzamiento;
}

QPointF Jugador::obtenerPosicionLanzamiento() const
{
    return canon->mapToScene(canon->line().p2());
}

QVector2D Jugador::obtenerVelocidadLanzamiento() const
{
    qreal anguloRadianes =
        qDegreesToRadians(anguloLanzamiento);

    qreal direccion = mirandoDerecha ? 1.0 : -1.0;

    qreal velocidadX =
        direccion * rapidezLanzamiento * qCos(anguloRadianes);

    qreal velocidadY =
        -rapidezLanzamiento * qSin(anguloRadianes);

    return QVector2D(velocidadX, velocidadY);
}

