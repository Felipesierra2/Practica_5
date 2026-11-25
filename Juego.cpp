#include "juego.h"

#include <QDebug>
#include <QtMath>
#include <QPen>

Juego::Juego(QWidget* parent)
    : QGraphicsView(parent),
    escena(new QGraphicsScene(this)),
    temporizador(new QTimer(this)),
    particulaActual(nullptr),
    posicionLanzamiento(50, 550),
    anguloLanzamiento(45.0),
    rapidezLanzamiento(20.0)
{
    inicializarEscenario();
    crearParedes();
    crearObstaculos();
    crearParticula();
    crearIndicadores();

    connect(temporizador, &QTimer::timeout,
            this, &Juego::actualizarJuego);

    reloj.start();
    temporizador->start(16);
}

void Juego::inicializarEscenario()
{
    escena->setSceneRect(0, 0, 800, 600);

    setScene(escena);

    setFixedSize(816, 639);

    setBackgroundBrush(QColor(28, 32, 45));

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setFocusPolicy(Qt::StrongFocus);
}

void Juego::crearParticula()
{
    particulaActual = new Particula(
        1.0,
        10.0,
        posicionLanzamiento
        );

    escena->addItem(particulaActual);

    particulas.append(particulaActual);
}

void Juego::keyPressEvent(QKeyEvent* evento)
{
    switch (evento->key())
    {
    case Qt::Key_Up:
        anguloLanzamiento += 5.0;
        break;

    case Qt::Key_Down:
        anguloLanzamiento -= 5.0;
        break;

    case Qt::Key_Right:
        rapidezLanzamiento += 1.0;
        break;

    case Qt::Key_Left:
        rapidezLanzamiento -= 1.0;
        break;

    case Qt::Key_Space:
        if (!evento->isAutoRepeat())
            dispararParticula();

        return;

    default:
        QGraphicsView::keyPressEvent(evento);
        return;
    }

    anguloLanzamiento = qBound(0.0, anguloLanzamiento, 90.0);

    rapidezLanzamiento = qBound(1.0, rapidezLanzamiento, 40.0);

    actualizarIndicadores();
}

void Juego::actualizarJuego()
{
    qreal deltaTiempo = reloj.restart() / 1000.0;

    deltaTiempo = qMin(deltaTiempo, 0.05);

    for (Particula* particula : particulas)
    {
        particula->actualizarMovimiento(deltaTiempo);
    }

    comprobarColisionesParedes();
    comprobarColisionesObstaculos();
}

void Juego::dispararParticula()
{
    if (!particulaActual)
        return;

    particulaActual->lanzar(rapidezLanzamiento,
                            anguloLanzamiento);

    crearParticula();
}

void Juego::crearIndicadores()
{
    indicadorLanzamiento = escena->addLine(
        QLineF(),
        QPen(Qt::green, 2)
        );

    informacionLanzamiento = escena->addText("");

    informacionLanzamiento->setDefaultTextColor(Qt::white);
    informacionLanzamiento->setPos(10, 10);

    actualizarIndicadores();
}

void Juego::actualizarIndicadores()
{
    qreal anguloRadianes =
        qDegreesToRadians(anguloLanzamiento);

    qreal longitud = rapidezLanzamiento * 4.0;

    QPointF extremo(
        posicionLanzamiento.x() +
            longitud * qCos(anguloRadianes),

        posicionLanzamiento.y() -
            longitud * qSin(anguloRadianes)
        );

    indicadorLanzamiento->setLine(
        QLineF(posicionLanzamiento, extremo)
        );

    informacionLanzamiento->setPlainText(
        QString("Angulo: %1 grados\nRapidez: %2 m/s")
            .arg(anguloLanzamiento)
            .arg(rapidezLanzamiento)
        );
}

void Juego::crearParedes()
{
    const QRectF limites = escena->sceneRect();

    const qreal ancho = limites.width();
    const qreal alto = limites.height();
    const qreal grosor = 15.0;

    paredes.append(new Pared(
        QRectF(0, grosor, grosor, alto - 2 * grosor),
        TipoPared::IZQUIERDA
        ));

    paredes.append(new Pared(
        QRectF(ancho - grosor, grosor,
               grosor, alto - 2 * grosor),
        TipoPared::DERECHA
        ));

    paredes.append(new Pared(
        QRectF(0, 0, ancho, grosor),
        TipoPared::SUPERIOR
        ));

    paredes.append(new Pared(
        QRectF(0, alto - grosor, ancho, grosor),
        TipoPared::INFERIOR
        ));

    for (Pared* pared : paredes)
    {
        escena->addItem(pared);
    }
}

void Juego::crearObstaculos()
{
    obstaculos.append(new Obstaculo(
        QRectF(250, 400, 80, 80), 0.8
        ));

    obstaculos.append(new Obstaculo(
        QRectF(450, 250, 80, 80), 0.6
        ));

    obstaculos.append(new Obstaculo(
        QRectF(600, 450, 80, 80), 0.4
        ));

    for (Obstaculo* obstaculo : obstaculos)
    {
        escena->addItem(obstaculo);
    }
}

void Juego::comprobarColisionesObstaculos()
{
    for (Particula* particula : particulas)
    {
        for (Obstaculo* obstaculo : obstaculos)
        {
            if (obstaculo->detectarColision(particula))
            {
                qreal energiaInicial =
                    particula->calcularEnergiaCinetica();

                obstaculo->resolverColision(particula);

                qreal energiaFinal =
                    particula->calcularEnergiaCinetica();

                if (energiaInicial - energiaFinal > 0.000001)
                {
                    qDebug() << "Colision con obstaculo."
                             << "Coeficiente:"
                             << obstaculo->obtenerCoeficienteRestitucion();

                    qDebug() << "Energia inicial:"
                             << energiaInicial << "J";

                    qDebug() << "Energia final:"
                             << energiaFinal << "J";

                    qDebug() << "Energia perdida:"
                             << energiaInicial - energiaFinal << "J";
                }
            }
        }
    }
}

void Juego::eliminarParticulasFueraEscenario()
{
    for (int i = particulas.size() - 1; i >= 0; i--)
    {
        Particula* particula = particulas.at(i);

        if (!particula->estaLanzada())
            continue;

        if (!escena->sceneRect().intersects(
                particula->sceneBoundingRect()))
        {
            particulas.removeAt(i);

            escena->removeItem(particula);

            delete particula;

            qDebug() << "Particula eliminada."
                     << "Particulas restantes:" << particulas.size();
        }
    }
}

void Juego::comprobarColisionesParedes()
{
    for (Particula* particula : particulas)
    {
        for (Pared* pared : paredes)
        {
            if (pared->detectarColision(particula))
            {
                qreal energiaInicial =
                    particula->calcularEnergiaCinetica();

                pared->resolverColision(particula);

                qreal energiaFinal =
                    particula->calcularEnergiaCinetica();

                qDebug() << "Colision con pared:"
                         << static_cast<int>(pared->obtenerTipo());

                qDebug() << "Energia inicial:" << energiaInicial
                         << "J";

                qDebug() << "Energia final:" << energiaFinal
                         << "J";

                qDebug() << "Diferencia:"
                         << energiaFinal - energiaInicial
                         << "J";
            }
        }
    }
}
