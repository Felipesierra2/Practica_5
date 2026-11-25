#include "particula.h"
#include <QBrush>
#include <QPen>
#include <QtMath>

Particula::Particula(qreal masa, qreal radio,
                     const QPointF& posicionInicial)
    : QGraphicsEllipseItem(-radio, -radio,
                           2 * radio, 2 * radio),
    masa(masa),
    radio(radio),
    velocidad(0, 0),
    aceleracion(0, GRAVEDAD),
    lanzada(false)
{
    setPos(posicionInicial);
    setBrush(Qt::blue);
    setPen(Qt::NoPen);
}

bool Particula::estaLanzada() const
{
    return lanzada;
}

qreal Particula::obtenerMasa() const
{
    return masa;
}

qreal Particula::obtenerRadio() const
{
    return radio;
}

QPointF Particula::obtenerVelocidad() const
{
    return velocidad;
}

void Particula::lanzar(qreal rapidezInicial, qreal angulo)
{
    if (lanzada)
        return;

    qreal anguloRadianes = qDegreesToRadians(angulo);

    velocidad.setX(rapidezInicial * qCos(anguloRadianes));
    velocidad.setY(-rapidezInicial * qSin(anguloRadianes));

    lanzada = true;
}

void Particula::actualizarMovimiento(qreal deltaTiempo)
{
    if (!lanzada)
        return;

    QPointF desplazamiento =
        velocidad * deltaTiempo +
        aceleracion * (0.5 * deltaTiempo * deltaTiempo);

    setPos(pos() + desplazamiento * PIXELES_POR_METRO);

    velocidad += aceleracion * deltaTiempo;
}
