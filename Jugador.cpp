#include "Jugador.h"
#include <QPen>
#include <QBrush>
#include <QtMath>
#include <qfont.h>

Jugador::Jugador(bool mirandoDerecha, const QColor &color, QGraphicsItem* parent)
    : QGraphicsItemGroup(parent),
    anguloLanzamiento(45.0),
    rapidezLanzamiento(20.0),
    mirandoDerecha(mirandoDerecha),
    vida(VIDA_MAXIMA)
{
    base = new QGraphicsEllipseItem(-20, -20, 40, 40);

    base->setBrush(color);
    base->setPen(QPen(color.lighter(150), 2));

    canon = new QGraphicsLineItem(0, 0, 35, 0);

    canon->setPen(QPen(color.darker(150), 8));

    addToGroup(base);
    addToGroup(canon);
    crearPlataforma();
    crearRepresentante(color);

    actualizarCanon();
    crearBarraVida();

    etiquetaNombre = new QGraphicsSimpleTextItem();

    etiquetaNombre->setBrush(color.lighter(160));
    etiquetaNombre->setFont(QFont("Arial", 10, QFont::Bold));

    addToGroup(etiquetaNombre);

    etiquetaNombre->setZValue(3);

    crearIndicadorTurno();
}

void Jugador::actualizarCanon()
{
    const qreal longitudCanon = 35.0;

    qreal anguloRadianes = qDegreesToRadians(anguloLanzamiento);

    qreal direccion = mirandoDerecha ? 1.0 : -1.0;

    qreal x = direccion * longitudCanon * qCos(anguloRadianes);
    qreal y = -longitudCanon * qSin(anguloRadianes);

    canon->setLine(0, 0, x, y);
}

void Jugador::modificarAngulo(qreal incremento)
{
    anguloLanzamiento = qBound(
        ANGULO_MINIMO,
        anguloLanzamiento + incremento,
        ANGULO_MAXIMO
        );

    actualizarCanon();
}

void Jugador::modificarRapidez(qreal incremento)
{
    rapidezLanzamiento = qBound(
        RAPIDEZ_MINIMA,
        rapidezLanzamiento + incremento,
        RAPIDEZ_MAXIMA
        );
}

void Jugador::recibirDano(qreal cantidad)
{
    if (cantidad <= 0 || estaDerrotado())
        return;

    vida = qMax(0.0, vida - cantidad);

    actualizarBarraVida();
}

void Jugador::reiniciar()
{
    vida = VIDA_MAXIMA;

    anguloLanzamiento = 45.0;
    rapidezLanzamiento = 20.0;

    actualizarCanon();
    actualizarBarraVida();
}

qreal Jugador::obtenerVida() const
{
    return vida;
}

bool Jugador::estaDerrotado() const
{
    return vida <= 0.0;
}

bool Jugador::detectarImpacto(
    const Particula* particula) const
{
    if (!particula || estaDerrotado())
        return false;

    return representante->collidesWithItem(
        particula,
        Qt::IntersectsItemShape
        );
}
qreal Jugador::obtenerAngulo() const
{
    return anguloLanzamiento;
}

qreal Jugador::obtenerRapidez() const
{
    return rapidezLanzamiento;
}

QPointF Jugador::obtenerPosicionLanzamiento() const
{
    return canon->mapToScene(canon->line().p2());
}

QVector2D Jugador::obtenerVelocidadLanzamiento() const
{
    qreal anguloRadianes =
        qDegreesToRadians(anguloLanzamiento);

    qreal direccion = mirandoDerecha ? 1.0 : -1.0;

    qreal velocidadX =
        direccion * rapidezLanzamiento * qCos(anguloRadianes);

    qreal velocidadY =
        -rapidezLanzamiento * qSin(anguloRadianes);

    return QVector2D(velocidadX, velocidadY);
}

void Jugador::establecerNombre(const QString& nombre)
{
    etiquetaNombre->setText(nombre);

    qreal anchoTexto = etiquetaNombre->boundingRect().width();

    etiquetaNombre->setPos(-anchoTexto / 2.0, -70);
}

void Jugador::crearIndicadorTurno()
{
    QPolygonF triangulo;

    triangulo << QPointF(-8, -90)
              << QPointF(8, -90)
              << QPointF(0, -79);

    indicadorTurno = new QGraphicsPolygonItem(triangulo);

    indicadorTurno->setBrush(QColor(250, 204, 21));
    indicadorTurno->setPen(Qt::NoPen);

    addToGroup(indicadorTurno);

    indicadorTurno->setZValue(4);
    indicadorTurno->setVisible(false);
}

void Jugador::establecerTurnoActivo(bool activo)
{
    indicadorTurno->setVisible(activo);
}

void Jugador::crearRepresentante(const QColor& color)
{
    representante = new QGraphicsRectItem(
        -15, -40, 30, 40
        );

    representante->setBrush(color);

    representante->setPen(
        QPen(color.lighter(150), 2)
        );

    addToGroup(representante);
}

void Jugador::establecerPosicionRepresentante(const QPointF& posicion)
{
    const QPointF posicionLocal = mapFromScene(posicion);

    representante->setPos(posicionLocal);

    fondoVida->setPos(posicionLocal);
    barraVida->setPos(posicionLocal);
}

void Jugador::crearPlataforma()
{
    plataforma = new QGraphicsRectItem(
        -45, 20, 90, 20
        );

    plataforma->setBrush(QColor(71, 85, 105));

    plataforma->setPen(
        QPen(QColor(148, 163, 184), 2)
        );

    addToGroup(plataforma);

    plataforma->setZValue(-1);
}
