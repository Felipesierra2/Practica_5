#ifndef PARTICULA_H
#define PARTICULA_H

#include <QGraphicsEllipseItem>
#include <QPointF>

class Particula : public QGraphicsEllipseItem
{
public:
    Particula(qreal masa, qreal radio,
              const QPointF& posicionInicial);

    Particula(qreal masa,
              qreal radio,
              const QPointF& posicionInicial,
              const QPointF& velocidadInicial);

    void lanzar(qreal rapidezInicial, qreal angulo);
    void lanzar(const QVector2D& velocidadInicial);
    void actualizarMovimiento(qreal deltaTiempo);
    void establecerVelocidad(const QPointF& nuevaVelocidad);

    bool estaLanzada() const;
    bool detectarColision(const Particula* otra) const;

    qreal obtenerMasa() const;
    qreal obtenerRadio() const;
    qreal calcularEnergiaCinetica() const;

    QPointF obtenerVelocidad() const;
    QPointF calcularMomentoLineal() const;

    int obtenerIdentificador() const;

private:
    qreal masa;
    qreal radio;

    QPointF velocidad;
    QPointF aceleracion;

    bool lanzada;

    static constexpr qreal GRAVEDAD = 9.81;
    static constexpr qreal PIXELES_POR_METRO = 20.0;

    const int identificador;
    static int siguienteIdentificador;
};

#endif // PARTICULA_H
