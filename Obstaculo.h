#ifndef OBSTACULO_H
#define OBSTACULO_H

#include <QGraphicsRectItem>
#include <QPointF>
#include "particula.h"

class Obstaculo : public QGraphicsRectItem
{
public:
    Obstaculo(const QRectF& dimensiones,
              qreal coeficienteRestitucion);

    bool detectarColision(const Particula* particula) const;

    void resolverColision(Particula* particula) const;

    qreal obtenerCoeficienteRestitucion() const;

private:
    qreal coeficienteRestitucion;

    QPointF calcularPuntoMasCercano(
        const QPointF& centro) const;

    QPointF calcularNormalColision(
        const Particula* particula) const;
};

#endif // OBSTACULO_H
