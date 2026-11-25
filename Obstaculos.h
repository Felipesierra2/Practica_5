#ifndef OBSTACULOS_H
#define OBSTACULOS_H

#include <QGraphicsRectItem>
#include <QRectF>
#include "Particula.h"

class Obstaculos : public QGraphicsRectItem {
public:
    Obstaculos(float x, float y, float ancho, float alto, float restitucion = 0.5f, QGraphicsItem* parent = nullptr);

    void verificarColision(Particula* p);

    void setRestitucion(float e) { coefRestitucion = e; }
    float getRestitucion() const { return coefRestitucion; }

private:
    QRectF geometria;
    float coefRestitucion;
};

#endif // OBSTACULOS_H
