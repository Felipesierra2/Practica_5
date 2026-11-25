#ifndef PARTICULA_H
#define PARTICULA_H

#include <QPointF>

class Particula
{
public:
    Particula(qreal masa, qreal radio,
              const QPointF& posicionInicial);

    Particula(qreal masa, qreal radio,
              const QPointF& posicionInicial,
              const QPointF& velocidadInicial);

    void lanzar(qreal rapidezInicial, qreal angulo);
    void actualizarMovimiento(qreal deltaTiempo);

    bool estaLanzada() const;

    qreal obtenerMasa() const;
    qreal obtenerRadio() const;

    QPointF obtenerVelocidad() const;
    void establecerVelocidad(const QPointF& nuevaVelocidad);

    QPointF pos() const;
    void setPos(const QPointF& nuevaPosicion);

    qreal calcularEnergiaCinetica() const;
    bool detectarColision(const Particula* otra) const;
    QPointF calcularMomentoLineal() const;

    int obtenerIdentificador() const;

private:
    qreal masa;
    qreal radio;

    QPointF posicion;
    QPointF velocidad;
    QPointF aceleracion;

    bool lanzada;

    const int identificador;
    static int siguienteIdentificador;

    static constexpr qreal GRAVEDAD = 9.81;
    static constexpr qreal PIXELES_POR_METRO = 20.0;
};

#endif
