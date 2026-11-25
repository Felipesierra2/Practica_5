#ifndef OBSTACULO_H
#define OBSTACULO_H

#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>
#include <QPointF>
#include "particula.h"

class Obstaculo : public QGraphicsRectItem
{
public:
    Obstaculo(const QRectF& dimensiones,
              qreal coeficienteRestitucion,
              qreal resistenciaInicial = 100.0,
              int propietario = 0);

    bool detectarColision(const Particula* particula) const;

    void resolverColision(Particula* particula) const;

    qreal obtenerCoeficienteRestitucion() const;

    qreal obtenerResistencia() const;
    qreal obtenerResistenciaMaxima() const;

    int obtenerPropietario() const;

    bool estaDestruido() const;

    void recibirDano(qreal cantidad);
    void reiniciar();

private:
    qreal coeficienteRestitucion;

    qreal resistenciaMaxima;
    qreal resistencia;

    int propietario;

    QGraphicsSimpleTextItem* textoResistencia;

    void crearTextoResistencia();
    void actualizarTextoResistencia();

    QPointF calcularPuntoMasCercano(
        const QPointF& centro) const;

    QPointF calcularNormalColision(
        const Particula* particula) const;
};

#endif // OBSTACULO_H
