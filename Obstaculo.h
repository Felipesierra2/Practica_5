#ifndef OBSTACULO_H
#define OBSTACULO_H

#include <QRectF>
#include <QPointF>
#include "particula.h"

class Obstaculo
{
public:
    Obstaculo(const QRectF& dimensiones,
              qreal coeficienteRestitucion);

    bool detectarColision(const Particula* particula) const;

    void resolverColision(Particula* particula) const;

    qreal obtenerCoeficienteRestitucion() const;

    QRectF obtenerLimites() const;

private:
    QRectF dimensiones;

    qreal coeficienteRestitucion;

    QPointF calcularPuntoMasCercano(
        const QPointF& centro) const;

    QPointF calcularNormalColision(
        const Particula* particula) const;
};

#endif
