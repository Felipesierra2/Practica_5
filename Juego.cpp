#include "juego.h"

#include <QDebug>
#include <QtMath>
#include <QPen>
#include <QDir>

Juego::Juego(QWidget* parent)
    : QGraphicsView(parent),
    escena(new QGraphicsScene(this)),
    temporizador(new QTimer(this)),
    particulaActual(nullptr),
    posicionLanzamiento(50, 550),
    anguloLanzamiento(45.0),
    rapidezLanzamiento(20.0),
    tiempoSimulacion(0.0)
{
    inicializarEscenario();
    crearParedes();
    crearObstaculos();
    crearParticula();
    crearIndicadores();

    connect(temporizador, &QTimer::timeout,
            this, &Juego::actualizarJuego);

    if (!registro.abrirArchivo("simulacion.txt"))
    {
        qDebug() << "No se pudo iniciar el registro.";
    }

    qDebug() << "Ruta del archivo:"
             << QDir::current().absoluteFilePath("simulacion.txt");

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

    tiempoSimulacion += deltaTiempo;

    for (Particula* particula : particulas)
    {
        particula->actualizarMovimiento(deltaTiempo);
    }

    comprobarColisionesParedes();

    comprobarColisionesObstaculos();

    comprobarColisionesParticulas();

    registrarPosiciones();
}

void Juego::dispararParticula()
{
    if (!particulaActual)
        return;

    particulaActual->lanzar(
        rapidezLanzamiento,
        anguloLanzamiento
        );

    registro.registrarPosicion(
        tiempoSimulacion,
        particulaActual->obtenerIdentificador(),
        particulaActual->scenePos()
        );

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

    obstaculos.append(new Obstaculo(
        QRectF(150, 150, 80, 80), 0.2
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
                const QPointF velocidadInicial =
                    particula->obtenerVelocidad();

                qreal energiaInicial =
                    particula->calcularEnergiaCinetica();

                obstaculo->resolverColision(particula);

                const QPointF velocidadFinal =
                    particula->obtenerVelocidad();

                qreal energiaFinal =
                    particula->calcularEnergiaCinetica();

                if (velocidadInicial == velocidadFinal)
                    continue;

                registro.registrarColision(
                    tiempoSimulacion,
                    particula->obtenerIdentificador(),
                    "OBSTACULO",
                    QString::number(
                        obstaculo->obtenerCoeficienteRestitucion()
                        )
                    );

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

void Juego::comprobarColisionesParticulas()
{
    for (int i = 0; i < particulas.size(); i++)
    {
        Particula* primera = particulas.at(i);

        for (int j = i + 1; j < particulas.size(); j++)
        {
            Particula* segunda = particulas.at(j);

            if (primera->detectarColision(segunda))
            {
                fusionarParticulas(i, j);
                return;
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
                const QPointF velocidadInicial =
                    particula->obtenerVelocidad();

                qreal energiaInicial =
                    particula->calcularEnergiaCinetica();

                pared->resolverColision(particula);

                const QPointF velocidadFinal =
                    particula->obtenerVelocidad();

                qreal energiaFinal =
                    particula->calcularEnergiaCinetica();

                if (velocidadInicial == velocidadFinal)
                    continue;

                registro.registrarColision(
                    tiempoSimulacion,
                    particula->obtenerIdentificador(),
                    "PARED",
                    pared->obtenerNombre()
                    );

                qDebug() << "Colision con pared:"
                         << pared->obtenerNombre();

                qDebug() << "Energia inicial:"
                         << energiaInicial << "J";

                qDebug() << "Energia final:"
                         << energiaFinal << "J";

                qDebug() << "Diferencia:"
                         << energiaFinal - energiaInicial
                         << "J";
            }
        }
    }
}

void Juego::fusionarParticulas(int indice1, int indice2)
{
    Particula* primera = particulas.at(indice1);
    Particula* segunda = particulas.at(indice2);

    registro.registrarPosicion(
        tiempoSimulacion,
        primera->obtenerIdentificador(),
        primera->scenePos()
        );

    registro.registrarPosicion(
        tiempoSimulacion,
        segunda->obtenerIdentificador(),
        segunda->scenePos()
        );

    const qreal energiaInicial =
        primera->calcularEnergiaCinetica() +
        segunda->calcularEnergiaCinetica();

    const qreal masa1 = primera->obtenerMasa();
    const qreal masa2 = segunda->obtenerMasa();

    const qreal masaFinal = masa1 + masa2;

    const QPointF momentoInicial =
        primera->calcularMomentoLineal() +
        segunda->calcularMomentoLineal();

    const QPointF velocidadFinal =
        momentoInicial / masaFinal;

    const qreal radio1 = primera->obtenerRadio();
    const qreal radio2 = segunda->obtenerRadio();

    const qreal radioFinal =
        qSqrt(radio1 * radio1 + radio2 * radio2);

    const QPointF posicionFinal =
        (primera->scenePos() * masa1 +
         segunda->scenePos() * masa2) / masaFinal;

    Particula* fusionada = new Particula(
        masaFinal,
        radioFinal,
        posicionFinal,
        velocidadFinal
        );

    registro.registrarFusion(
        tiempoSimulacion,
        primera->obtenerIdentificador(),
        segunda->obtenerIdentificador(),
        fusionada->obtenerIdentificador()
        );

    const QPointF momentoFinal =
        fusionada->calcularMomentoLineal();

    const qreal energiaFinal =
        fusionada->calcularEnergiaCinetica();

    const QPointF diferenciaMomento =
        momentoFinal - momentoInicial;

    const qreal energiaPerdida =
        energiaInicial - energiaFinal;

    // Retirar las particulas originales de la lista
    particulas.removeAt(indice2);
    particulas.removeAt(indice1);

    // Retirar las particulas de la escena
    escena->removeItem(primera);
    escena->removeItem(segunda);

    // Liberar la memoria
    delete primera;
    delete segunda;

    // Incorporar la nueva particula
    escena->addItem(fusionada);
    particulas.append(fusionada);


    qDebug() << "========== FUSION DE PARTICULAS ==========";

    qDebug() << "Masa resultante:"
             << masaFinal << "kg";

    qDebug() << "Radio resultante:"
             << radioFinal << "px";

    qDebug() << "Velocidad resultante:"
             << velocidadFinal << "m/s";

    qDebug() << "Momento lineal inicial:"
             << momentoInicial << "kg*m/s";

    qDebug() << "Momento lineal final:"
             << momentoFinal << "kg*m/s";

    qDebug() << "Diferencia de momento:"
             << diferenciaMomento << "kg*m/s";

    qDebug() << "Energia inicial:"
             << energiaInicial << "J";

    qDebug() << "Energia final:"
             << energiaFinal << "J";

    qDebug() << "Energia perdida:"
             << energiaPerdida << "J";

    qDebug() << "Particulas restantes:"
             << particulas.size();

    qDebug() << "=========================================";
}

void Juego::registrarPosiciones()
{
    for (Particula* particula : particulas)
    {
        if (!particula->estaLanzada())
            continue;

        registro.registrarPosicion(
            tiempoSimulacion,
            particula->obtenerIdentificador(),
            particula->scenePos()
            );
    }
}
