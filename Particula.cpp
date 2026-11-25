#include "particula.h"
#include <QBrush>
#include <QPen>
#include <QtMath>
#include <QDebug>
#include <qvectornd.h>
#include <QColor>

int Particula::siguienteIdentificador = 1;

Particula::Particula(
    qreal masa,
    qreal radio,
    const QPointF& posicionInicial)
    : QGraphicsEllipseItem(-radio, -radio,
                           2 * radio, 2 * radio),
    masa(masa),
    radio(radio),
    velocidad(0, 0),
    aceleracion(0, GRAVEDAD),
    propietario(0),
    lanzada(false),
    identificador(siguienteIdentificador++)
{
    setPos(posicionInicial);
    setBrush(Qt::blue);
    setPen(Qt::NoPen);

    qDebug() << "Particula creada. ID:" << identificador;
}

Particula::Particula(qreal masa,
                     qreal radio,
                     const QPointF& posicionInicial,
                     const QPointF& velocidadInicial)
    : Particula(masa, radio, posicionInicial)
{
    velocidad = velocidadInicial;
    lanzada = true;

    setBrush(Qt::green);
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

QPointF Particula::calcularMomentoLineal() const
{
    return velocidad * masa;
}

qreal Particula::calcularEnergiaCinetica() const
{
    return 0.5 * masa *
           (velocidad.x() * velocidad.x() +
            velocidad.y() * velocidad.y());
}

void Particula::lanzar(qreal rapidezInicial, qreal angulo)
{
    qreal anguloRadianes = qDegreesToRadians(angulo);

    QVector2D velocidadInicial(
        rapidezInicial * qCos(anguloRadianes),
        -rapidezInicial * qSin(anguloRadianes)
        );

    lanzar(velocidadInicial);
}

void Particula::lanzar(const QVector2D& velocidadInicial)
{
    if (lanzada)
        return;

    velocidad = velocidadInicial.toPointF();

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

void Particula::establecerVelocidad(
    const QPointF& nuevaVelocidad)
{
    velocidad = nuevaVelocidad;
}

int Particula::obtenerIdentificador() const
{
    return identificador;
}

void Particula::establecerPropietario(int nuevoPropietario)
{
    propietario = nuevoPropietario;

    if (propietario == 1)
        setBrush(QColor(37, 99, 235));

    else if (propietario == 2)
        setBrush(QColor(220, 38, 38));

    else
        setBrush(Qt::gray);
}

int Particula::obtenerPropietario() const
{
    return propietario;
}
