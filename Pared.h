#ifndef PARED_H
#define PARED_H

#include <QGraphicsRectItem>
#include "particula.h"

enum class TipoPared
{
    IZQUIERDA,
    DERECHA,
    SUPERIOR,
    INFERIOR
};

class Pared : public QGraphicsRectItem
{
public:
    Pared(const QRectF& dimensiones, TipoPared tipo);

    bool detectarColision(const Particula* particula) const;

    void resolverColision(Particula* particula) const;

    TipoPared obtenerTipo() const;

private:
    TipoPared tipo;

    static constexpr qreal COEFICIENTE_RESTITUCION = 1.0;
};

#endif // PARED_H

