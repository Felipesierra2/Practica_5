#include "particula.h"
#include <QtMath>

int Particula::siguienteIdentificador = 1;

Particula::Particula(qreal masa, qreal radio,
                     const QPointF& posicionInicial)
    : masa(masa),
    radio(radio),
    posicion(posicionInicial),
    velocidad(0, 0),
    aceleracion(0, GRAVEDAD),
    lanzada(false),
    identificador(siguienteIdentificador++)
{
}

Particula::Particula(qreal masa,
                     qreal radio,
                     const QPointF& posicionInicial,
                     const QPointF& velocidadInicial)
    : Particula(masa, radio, posicionInicial)
{
    velocidad = velocidadInicial;
    lanzada = true;
}

bool Particula::estaLanzada() const
{
    return lanzada;
}

QPointF Particula::pos() const
{
    return posicion;
}

void Particula::setPos(const QPointF& nuevaPosicion)
{
    posicion = nuevaPosicion;
}

bool Particula::detectarColision(
    const Particula* otra) const
{
    if (!otra || otra == this)
        return false;

    if (!estaLanzada() || !otra->estaLanzada())
        return false;

    const QPointF centro1 = pos();
    const QPointF centro2 = otra->pos();

    const qreal dx = centro2.x() - centro1.x();
    const qreal dy = centro2.y() - centro1.y();

    const qreal distanciaCuadrada =
        dx * dx + dy * dy;

    const qreal sumaRadios =
        radio + otra->obtenerRadio();

    return distanciaCuadrada <=
           sumaRadios * sumaRadios;
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

void Particula::establecerVelocidad(
    const QPointF& nuevaVelocidad)
{
    velocidad = nuevaVelocidad;
}

int Particula::obtenerIdentificador() const
{
    return identificador;
}
