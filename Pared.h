#ifndef PARED_H
#define PARED_H

#include <QRectF>
#include <QString>
#include "particula.h"

enum class TipoPared
{
    IZQUIERDA,
    DERECHA,
    SUPERIOR,
    INFERIOR
};

class Pared
{
public:
    Pared(const QRectF& dimensiones, TipoPared tipo);

    bool detectarColision(const Particula* particula) const;

    void resolverColision(Particula* particula) const;

    TipoPared obtenerTipo() const;

    QString obtenerNombre() const;

    QRectF obtenerLimites() const;

private:
    QRectF dimensiones;

    TipoPared tipo;

    static constexpr qreal COEFICIENTE_RESTITUCION = 1.0;
};

#endif

