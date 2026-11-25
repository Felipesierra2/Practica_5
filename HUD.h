#ifndef HUD_H
#define HUD_H

#include <QGraphicsItemGroup>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsRectItem>

class HUD : public QGraphicsItemGroup
{
public:
    explicit HUD(QGraphicsItem* parent = nullptr);

    void actualizarTurno(int numeroJugador);
    void actualizarAngulo(qreal angulo);
    void actualizarRapidez(qreal rapidez);

private:
    QGraphicsSimpleTextItem* textoTurno;
    QGraphicsSimpleTextItem* textoAngulo;
    QGraphicsSimpleTextItem* textoRapidez;
    QGraphicsSimpleTextItem* textoControles;

    QGraphicsRectItem* fondo;

    void crearFondo();
    void configurarTextos();
};

#endif // HUD_H
