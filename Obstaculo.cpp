#include "obstaculo.h"
#include <QBrush>
#include <QPen>
#include <QtMath>
#include <QDebug>

Obstaculo::Obstaculo(const QRectF& dimensiones,
                     qreal coeficienteRestitucion)
    : QGraphicsRectItem(dimensiones),
    coeficienteRestitucion(
        qBound(0.0, coeficienteRestitucion, 1.0))
{
    setBrush(QColor(162, 120, 76));
    setPen(Qt::NoPen);
}

qreal Obstaculo::obtenerCoeficienteRestitucion() const
{
    return coeficienteRestitucion;
}

QPointF Obstaculo::calcularPuntoMasCercano(
    const QPointF& centro) const
{
    const QRectF limites = sceneBoundingRect();

    qreal x = qBound(
        limites.left(),
        centro.x(),
        limites.right()
        );

    qreal y = qBound(
        limites.top(),
        centro.y(),
        limites.bottom()
        );

    return QPointF(x, y);
}

bool Obstaculo::detectarColision(
    const Particula* particula) const
{
    if (!particula || !particula->estaLanzada())
        return false;

    const QPointF centro = particula->scenePos();

    const QPointF puntoCercano =
        calcularPuntoMasCercano(centro);

    const qreal dx = centro.x() - puntoCercano.x();
    const qreal dy = centro.y() - puntoCercano.y();

    const qreal distanciaCuadrada = dx * dx + dy * dy;

    const qreal radio = particula->obtenerRadio();

    return distanciaCuadrada <= radio * radio;
}

QPointF Obstaculo::calcularNormalColision(
    const Particula* particula) const
{
    const QPointF centro = particula->scenePos();

    const QPointF puntoCercano =
        calcularPuntoMasCercano(centro);

    QPointF normal = centro - puntoCercano;

    qreal magnitud = qSqrt(
        normal.x() * normal.x() +
        normal.y() * normal.y()
        );

    if (magnitud > 0.000001)
    {
        return normal / magnitud;
    }

    const QRectF limites = sceneBoundingRect();

    qreal distanciaIzquierda =
        centro.x() - limites.left();

    qreal distanciaDerecha =
        limites.right() - centro.x();

    qreal distanciaSuperior =
        centro.y() - limites.top();

    qreal distanciaInferior =
        limites.bottom() - centro.y();

    qreal distanciaMinima = distanciaIzquierda;

    normal = QPointF(-1, 0);

    if (distanciaDerecha < distanciaMinima)
    {
        distanciaMinima = distanciaDerecha;
        normal = QPointF(1, 0);
    }

    if (distanciaSuperior < distanciaMinima)
    {
        distanciaMinima = distanciaSuperior;
        normal = QPointF(0, -1);
    }

    if (distanciaInferior < distanciaMinima)
    {
        normal = QPointF(0, 1);
    }

    return normal;
}

void Obstaculo::resolverColision(Particula* particula) const
{
    if (!detectarColision(particula))
        return;

    const QPointF centro = particula->scenePos();

    const QPointF normal =
        calcularNormalColision(particula);

    const QPointF puntoCercano =
        calcularPuntoMasCercano(centro);

    const QPointF diferencia = centro - puntoCercano;

    const qreal distancia = qSqrt(
        diferencia.x() * diferencia.x() +
        diferencia.y() * diferencia.y()
        );

    const qreal radio = particula->obtenerRadio();

    qreal penetracion = 0;

    if (distancia > 0.000001)
    {
        penetracion = radio - distancia;
    }
    else
    {
        const QRectF limites = sceneBoundingRect();

        if (normal.x() < 0)
            penetracion = radio + centro.x() - limites.left();

        else if (normal.x() > 0)
            penetracion = radio + limites.right() - centro.x();

        else if (normal.y() < 0)
            penetracion = radio + centro.y() - limites.top();

        else
            penetracion = radio + limites.bottom() - centro.y();
    }

    particula->setPos(
        centro + normal * (penetracion + 0.001)
        );

    QPointF velocidad = particula->obtenerVelocidad();

    qreal velocidadNormal =
        velocidad.x() * normal.x() +
        velocidad.y() * normal.y();

    if (velocidadNormal >= 0)
        return;

    QPointF nuevaVelocidad =
        velocidad -
        normal * ((1.0 + coeficienteRestitucion) *
                  velocidadNormal);

    particula->establecerVelocidad(nuevaVelocidad);
}
