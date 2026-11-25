#include "pared.h"

Pared::Pared(const QRectF& dimensiones, TipoPared tipo)
    : dimensiones(dimensiones),
    tipo(tipo)
{
}

QRectF Pared::obtenerLimites() const
{
    return dimensiones;
}

TipoPared Pared::obtenerTipo() const
{
    return tipo;
}

bool Pared::detectarColision(
    const Particula* particula) const
{
    if (!particula || !particula->estaLanzada())
        return false;

    const QPointF centro = particula->pos();

    const qreal radio = particula->obtenerRadio();

    const QRectF limites = obtenerLimites();

    switch (tipo)
    {
    case TipoPared::IZQUIERDA:
        return centro.x() - radio <= limites.right();

    case TipoPared::DERECHA:
        return centro.x() + radio >= limites.left();

    case TipoPared::SUPERIOR:
        return centro.y() - radio <= limites.bottom();

    case TipoPared::INFERIOR:
        return centro.y() + radio >= limites.top();
    }

    return false;
}

void Pared::resolverColision(Particula* particula) const
{
    if (!detectarColision(particula))
        return;

    QPointF posicion = particula->pos();
    QPointF velocidad = particula->obtenerVelocidad();

    const qreal radio = particula->obtenerRadio();
    const QRectF limites = obtenerLimites();

    switch (tipo)
    {
    case TipoPared::IZQUIERDA:

        posicion.setX(limites.right() + radio);

        if (velocidad.x() < 0)
            velocidad.setX(-velocidad.x() *
                           COEFICIENTE_RESTITUCION);

        break;

    case TipoPared::DERECHA:

        posicion.setX(limites.left() - radio);

        if (velocidad.x() > 0)
            velocidad.setX(-velocidad.x() *
                           COEFICIENTE_RESTITUCION);

        break;

    case TipoPared::SUPERIOR:

        posicion.setY(limites.bottom() + radio);

        if (velocidad.y() < 0)
            velocidad.setY(-velocidad.y() *
                           COEFICIENTE_RESTITUCION);

        break;

    case TipoPared::INFERIOR:

        posicion.setY(limites.top() - radio);

        if (velocidad.y() > 0)
            velocidad.setY(-velocidad.y() *
                           COEFICIENTE_RESTITUCION);

        break;
    }

    particula->setPos(posicion);
    particula->establecerVelocidad(velocidad);
}

QString Pared::obtenerNombre() const
{
    switch (tipo)
    {
    case TipoPared::IZQUIERDA:
        return "IZQUIERDA";

    case TipoPared::DERECHA:
        return "DERECHA";

    case TipoPared::SUPERIOR:
        return "SUPERIOR";

    case TipoPared::INFERIOR:
        return "INFERIOR";
    }

    return "DESCONOCIDA";
}
