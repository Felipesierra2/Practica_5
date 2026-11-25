#ifndef PAREDES_H
#define PAREDES_H

#include <QGraphicsRectItem>
#include <QRectF>
#include "Particula.h"

class Paredes : public QGraphicsRectItem {
public:
    Paredes(float x, float y, float ancho, float alto, QGraphicsItem* parent = nullptr);

    void verificarColision(Particula* p);

    QRectF getLimites() const { return limites; }

private:
    QRectF limites;
};

#endif // PAREDES_H
