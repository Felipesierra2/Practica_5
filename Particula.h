#ifndef PARTICULA_H
#define PARTICULA_H

#include <QGraphicsEllipseItem>
#include <QPointF>

class Particula : public QGraphicsEllipseItem
{
public:
    Particula(qreal masa, qreal radio,
              const QPointF& posicionInicial);

    void lanzar(qreal rapidezInicial, qreal angulo);
    void actualizarMovimiento(qreal deltaTiempo);
    void establecerVelocidad(const QPointF& nuevaVelocidad);

    bool estaLanzada() const;

    qreal obtenerMasa() const;
    qreal obtenerRadio() const;
    qreal calcularEnergiaCinetica() const;

    QPointF obtenerVelocidad() const;

private:
    qreal masa;
    qreal radio;

    QPointF velocidad;
    QPointF aceleracion;

    bool lanzada;

    static constexpr qreal GRAVEDAD = 9.81;
    static constexpr qreal PIXELES_POR_METRO = 20.0;
};

#endif // PARTICULA_H
