#include "obstaculo.h"
#include <QBrush>
#include <QPen>
#include <QtMath>
#include <QDebug>
#include <QFont>

Obstaculo::Obstaculo(const QRectF& dimensiones,
                     qreal coeficienteRestitucion,
                     qreal resistenciaInicial,
                     int propietario)
    : QGraphicsRectItem(dimensiones),
    coeficienteRestitucion(
        qBound(0.0, coeficienteRestitucion, 1.0)),
    resistenciaMaxima(qMax(1.0, resistenciaInicial)),
    resistencia(resistenciaMaxima),
    propietario(propietario)
{
    setBrush(QColor(162, 120, 76));
    setPen(Qt::NoPen);

    crearTextoResistencia();
}
qreal Obstaculo::obtenerCoeficienteRestitucion() const
{
    return coeficienteRestitucion;
}

QPointF Obstaculo::calcularPuntoMasCercano(
    const QPointF& centro) const
{
    const QRectF limites = sceneBoundingRect();

    qreal x = qBound(
        limites.left(),
        centro.x(),
        limites.right()
        );

    qreal y = qBound(
        limites.top(),
        centro.y(),
        limites.bottom()
        );

    return QPointF(x, y);
}

bool Obstaculo::detectarColision(const Particula* particula) const
{
    if (!particula || !particula->estaLanzada() || estaDestruido())
        return false;

    const QPointF centro = particula->scenePos();

    const QPointF puntoCercano =
        calcularPuntoMasCercano(centro);

    const qreal dx = centro.x() - puntoCercano.x();
    const qreal dy = centro.y() - puntoCercano.y();

    const qreal distanciaCuadrada = dx * dx + dy * dy;

    const qreal radio = particula->obtenerRadio();

    return distanciaCuadrada <= radio * radio;
}

QPointF Obstaculo::calcularNormalColision(
    const Particula* particula) const
{
    const QPointF centro = particula->scenePos();

    const QPointF puntoCercano =
        calcularPuntoMasCercano(centro);

    QPointF normal = centro - puntoCercano;

    qreal magnitud = qSqrt(
        normal.x() * normal.x() +
        normal.y() * normal.y()
        );

    if (magnitud > 0.000001)
    {
        return normal / magnitud;
    }

    const QRectF limites = sceneBoundingRect();

    qreal distanciaIzquierda =
        centro.x() - limites.left();

    qreal distanciaDerecha =
        limites.right() - centro.x();

    qreal distanciaSuperior =
        centro.y() - limites.top();

    qreal distanciaInferior =
        limites.bottom() - centro.y();

    qreal distanciaMinima = distanciaIzquierda;

    normal = QPointF(-1, 0);

    if (distanciaDerecha < distanciaMinima)
    {
        distanciaMinima = distanciaDerecha;
        normal = QPointF(1, 0);
    }

    if (distanciaSuperior < distanciaMinima)
    {
        distanciaMinima = distanciaSuperior;
        normal = QPointF(0, -1);
    }

    if (distanciaInferior < distanciaMinima)
    {
        normal = QPointF(0, 1);
    }

    return normal;
}

void Obstaculo::resolverColision(Particula* particula) const
{
    if (!detectarColision(particula))
        return;

    const QPointF centro = particula->scenePos();

    const QPointF normal =
        calcularNormalColision(particula);

    const QPointF puntoCercano =
        calcularPuntoMasCercano(centro);

    const QPointF diferencia = centro - puntoCercano;

    const qreal distancia = qSqrt(
        diferencia.x() * diferencia.x() +
        diferencia.y() * diferencia.y()
        );

    const qreal radio = particula->obtenerRadio();

    qreal penetracion = 0;

    if (distancia > 0.000001)
    {
        penetracion = radio - distancia;
    }
    else
    {
        const QRectF limites = sceneBoundingRect();

        if (normal.x() < 0)
            penetracion = radio + centro.x() - limites.left();

        else if (normal.x() > 0)
            penetracion = radio + limites.right() - centro.x();

        else if (normal.y() < 0)
            penetracion = radio + centro.y() - limites.top();

        else
            penetracion = radio + limites.bottom() - centro.y();
    }

    particula->setPos(
        centro + normal * (penetracion + 0.001)
        );

    QPointF velocidad = particula->obtenerVelocidad();

    qreal velocidadNormal =
        velocidad.x() * normal.x() +
        velocidad.y() * normal.y();

    if (velocidadNormal >= 0)
        return;

    QPointF nuevaVelocidad =
        velocidad -
        normal * ((1.0 + coeficienteRestitucion) *
                  velocidadNormal);

    particula->establecerVelocidad(nuevaVelocidad);
}


qreal Obstaculo::obtenerResistencia() const
{
    return resistencia;
}

qreal Obstaculo::obtenerResistenciaMaxima() const
{
    return resistenciaMaxima;
}

int Obstaculo::obtenerPropietario() const
{
    return propietario;
}

bool Obstaculo::estaDestruido() const
{
    return resistencia <= 0.0;
}

void Obstaculo::recibirDano(qreal cantidad)
{
    if (cantidad <= 0.0 || estaDestruido())
        return;

    resistencia = qMax(0.0, resistencia - cantidad);

    actualizarTextoResistencia();

    qDebug() << "Infraestructura del jugador:"
             << propietario
             << "| Dano recibido:" << cantidad
             << "| Resistencia restante:" << resistencia;

    if (estaDestruido())
    {
        setVisible(false);

        qDebug() << "Infraestructura destruida.";
    }
}

void Obstaculo::reiniciar()
{
    resistencia = resistenciaMaxima;

    actualizarTextoResistencia();

    setVisible(true);
}

void Obstaculo::crearTextoResistencia()
{
    textoResistencia = new QGraphicsSimpleTextItem(this);

    textoResistencia->setFont(
        QFont("Arial", 14, QFont::Bold)
        );

    textoResistencia->setZValue(1);

    actualizarTextoResistencia();
}

void Obstaculo::actualizarTextoResistencia()
{
    textoResistencia->setText(
        QString::number(qCeil(resistencia))
        );

    const QRectF limitesTexto =
        textoResistencia->boundingRect();

    const QPointF centro = rect().center();

    textoResistencia->setPos(
        centro.x() - limitesTexto.center().x(),
        centro.y() - limitesTexto.center().y()
        );

    const qreal proporcion =
        resistencia / resistenciaMaxima;

    if (proporcion > 0.6)
        textoResistencia->setBrush(Qt::white);

    else if (proporcion > 0.3)
        textoResistencia->setBrush(Qt::yellow);

    else
        textoResistencia->setBrush(QColor(255, 100, 100));
}
