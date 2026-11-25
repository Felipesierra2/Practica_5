#include "HUD.h"
#include <qgraphicsitem.h>
#include <QFont>
#include <QPen>
#include <QBrush>

HUD::HUD(QGraphicsItem* parent)
    : QGraphicsItemGroup(parent)
{
    crearFondo();

    textoTurno = new QGraphicsSimpleTextItem();
    textoAngulo = new QGraphicsSimpleTextItem();
    textoRapidez = new QGraphicsSimpleTextItem();
    textoControles = new QGraphicsSimpleTextItem();

    addToGroup(textoTurno);
    addToGroup(textoAngulo);
    addToGroup(textoRapidez);
    addToGroup(textoControles);

    configurarTextos();

    setZValue(50);
}

void HUD::crearFondo()
{
    fondo = new QGraphicsRectItem(0, 0, 720, 80);

    fondo->setBrush(QColor(32, 41, 56));
    fondo->setPen(QPen(QColor(82, 97, 118), 2));

    addToGroup(fondo);

    fondo->setZValue(0);
}

void HUD::configurarTextos()
{
    QFont fuente("Arial", 12, QFont::Bold);

    textoTurno->setFont(fuente);
    textoAngulo->setFont(fuente);
    textoRapidez->setFont(fuente);

    textoTurno->setBrush(QColor(96, 165, 250));
    textoAngulo->setBrush(Qt::white);
    textoRapidez->setBrush(Qt::white);

    textoControles->setFont(QFont("Arial", 10));
    textoControles->setBrush(QColor(203, 213, 225));

    textoTurno->setPos(15, 20);
    textoAngulo->setPos(180, 20);
    textoRapidez->setPos(310, 20);
    textoControles->setPos(470, 8);

    textoTurno->setZValue(1);
    textoAngulo->setZValue(1);
    textoRapidez->setZValue(1);
    textoControles->setZValue(1);

    textoTurno->setText("Jugador 1");
    textoAngulo->setText("Angulo: 45°");
    textoRapidez->setText("Rapidez: 20 m/s");

    textoControles->setText(
        "Arriba / Abajo: Angulo\n"
        "Izquierda / Derecha: Rapidez\n"
        "Espacio: Disparar"
        );
}

void HUD::actualizarTurno(int numeroJugador)
{
    textoTurno->setText(
        QString("Jugador %1").arg(numeroJugador)
        );

    if (numeroJugador == 1)
        textoTurno->setBrush(QColor(96, 165, 250));
    else
        textoTurno->setBrush(QColor(248, 113, 113));
}

void HUD::actualizarAngulo(qreal angulo)
{
    textoAngulo->setText(
        QString("Angulo: %1°").arg(angulo, 0, 'f', 0)
        );
}

void HUD::actualizarRapidez(qreal rapidez)
{
    textoRapidez->setText(
        QString("Rapidez: %1 m/s").arg(rapidez, 0, 'f', 0)
        );
}
