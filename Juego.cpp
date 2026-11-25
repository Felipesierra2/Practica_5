#include "juego.h"

#include <QDebug>
#include <QtMath>
#include <QPen>
#include <QDir>
#include <QFont>
#include <QBrush>

Juego::Juego(QWidget* parent)
    : QGraphicsView(parent),
    escena(new QGraphicsScene(this)),
    temporizador(new QTimer(this)),
    particulaActual(nullptr),
    posicionLanzamiento(50, 550),
    anguloLanzamiento(45.0),
    rapidezLanzamiento(20.0),
    tiempoSimulacion(0.0),
    partidaFinalizada(false)
{
    inicializarEscenario();
    crearJugadores();
    crearParedes();
    crearObstaculos();
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
    if (!jugadorActual)
        return;

    QPointF posicionInicial =
        jugadorActual->obtenerPosicionLanzamiento();

    particulaActual = new Particula(
        1.0,
        10.0,
        posicionInicial
        );

    escena->addItem(particulaActual);

    particulas.append(particulaActual);
}

void Juego::keyPressEvent(QKeyEvent* evento)
{
    if (partidaFinalizada || !jugadorActual)
        return;

    switch (evento->key())
    {
    case Qt::Key_Up:
        jugadorActual->modificarAngulo(5.0);
        break;

    case Qt::Key_Down:
        jugadorActual->modificarAngulo(-5.0);
        break;

    case Qt::Key_Right:
        jugadorActual->modificarRapidez(1.0);
        break;

    case Qt::Key_Left:
        jugadorActual->modificarRapidez(-1.0);
        break;

    case Qt::Key_Space:
        if (!evento->isAutoRepeat())
            dispararParticula();

        return;

    default:
        QGraphicsView::keyPressEvent(evento);
        return;
    }

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

    comprobarColisionesJugadores();

    registrarPosiciones();

    comprobarFinPartida();
}

void Juego::dispararParticula()
{
    if (!jugadorActual)
        return;

    crearParticula();

    if (!particulaActual)
        return;

    particulaActual->lanzar(
        jugadorActual->obtenerVelocidadLanzamiento()
        );

    registro.registrarPosicion(
        tiempoSimulacion,
        particulaActual->obtenerIdentificador(),
        particulaActual->scenePos()
        );

    particulaActual = nullptr;

    cambiarTurno();
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
    if (!jugadorActual)
        return;

    QPointF origen =
        jugadorActual->obtenerPosicionLanzamiento();

    QVector2D velocidad =
        jugadorActual->obtenerVelocidadLanzamiento();

    QPointF extremo =
        origen + (velocidad * 4.0f).toPointF();

    indicadorLanzamiento->setLine(
        QLineF(origen, extremo)
        );

    informacionLanzamiento->setPlainText(
        QString("Turno: Jugador %1\n"
                "Angulo: %2 grados\n"
                "Rapidez: %3 m/s")
            .arg(jugadorActual == jugador1 ? 1 : 2)
            .arg(jugadorActual->obtenerAngulo())
            .arg(jugadorActual->obtenerRapidez())
        );
}

void Jugador::crearBarraVida()
{
    fondoVida = new QGraphicsRectItem(
        -ANCHO_BARRA_VIDA / 2.0,
        -50.0,
        ANCHO_BARRA_VIDA,
        ALTO_BARRA_VIDA
        );

    fondoVida->setBrush(Qt::darkGray);
    fondoVida->setPen(Qt::NoPen);

    barraVida = new QGraphicsRectItem(
        fondoVida->rect()
        );

    barraVida->setBrush(Qt::green);
    barraVida->setPen(Qt::NoPen);

    addToGroup(fondoVida);
    addToGroup(barraVida);

    fondoVida->setZValue(1);
    barraVida->setZValue(2);
}

void Jugador::actualizarBarraVida()
{
    qreal proporcion = vida / VIDA_MAXIMA;

    barraVida->setRect(
        -ANCHO_BARRA_VIDA / 2.0,
        -50.0,
        ANCHO_BARRA_VIDA * proporcion,
        ALTO_BARRA_VIDA
        );

    if (proporcion > 0.6)
        barraVida->setBrush(Qt::green);

    else if (proporcion > 0.3)
        barraVida->setBrush(Qt::yellow);

    else
        barraVida->setBrush(Qt::red);
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

void Juego::cambiarTurno()
{
    if (jugadorActual == jugador1)
        jugadorActual = jugador2;
    else
        jugadorActual = jugador1;

    actualizarIndicadores();
}

void Juego::crearJugadores()
{
    jugador1 = new Jugador(true);
    jugador2 = new Jugador(false);

    QRectF limites = escena->sceneRect();

    jugador1->setPos(
        limites.left() + 60,
        limites.bottom() - 60
        );

    jugador2->setPos(
        limites.right() - 60,
        limites.bottom() - 60
        );

    escena->addItem(jugador1);
    escena->addItem(jugador2);

    jugadorActual = jugador1;

    jugador1->recibirDano(25.0);
    jugador2->recibirDano(80.0);
}

void Juego::comprobarColisionesJugadores()
{
    for (int i = particulas.size() - 1; i >= 0; i--)
    {
        Particula* particula = particulas.at(i);

        if (!particula->estaLanzada())
            continue;

        Jugador* jugadorImpactado = nullptr;

        if (jugador1->detectarImpacto(particula))
            jugadorImpactado = jugador1;

        else if (jugador2->detectarImpacto(particula))
            jugadorImpactado = jugador2;

        if (!jugadorImpactado)
            continue;

        const qreal energia =
            particula->calcularEnergiaCinetica();

        const qreal dano = energia * FACTOR_DANO;

        jugadorImpactado->recibirDano(dano);

        registro.registrarPosicion(
            tiempoSimulacion,
            particula->obtenerIdentificador(),
            particula->scenePos()
            );

        qDebug() << "========== IMPACTO ==========";

        qDebug() << "Jugador impactado:"
                 << (jugadorImpactado == jugador1 ? 1 : 2);

        qDebug() << "Masa:"
                 << particula->obtenerMasa() << "kg";

        qDebug() << "Energia cinetica:"
                 << energia << "J";

        qDebug() << "Dano generado:" << dano;

        qDebug() << "Vida restante:"
                 << jugadorImpactado->obtenerVida();

        qDebug() << "=============================";

        particulas.removeAt(i);

        escena->removeItem(particula);

        delete particula;
    }
}

void Juego::comprobarFinPartida()
{
    if (partidaFinalizada)
        return;

    if (jugador1->estaDerrotado() ||
        jugador2->estaDerrotado())
    {
        finalizarPartida();
    }
}

void Juego::finalizarPartida()
{
    if (partidaFinalizada)
        return;

    partidaFinalizada = true;

    temporizador->stop();

    indicadorLanzamiento->setVisible(false);

    QString mensaje;

    if (jugador1->estaDerrotado() &&
        jugador2->estaDerrotado())
    {
        mensaje = "EMPATE";
    }
    else if (jugador1->estaDerrotado())
    {
        mensaje = "JUGADOR 2 GANA";
    }
    else
    {
        mensaje = "JUGADOR 1 GANA";
    }

    QGraphicsTextItem* resultado =
        escena->addText(mensaje);

    resultado->setFont(
        QFont("Arial", 28, QFont::Bold)
        );

    resultado->setDefaultTextColor(Qt::yellow);

    QRectF limites = resultado->boundingRect();

    resultado->setPos(
        escena->sceneRect().center().x()
            - limites.width() / 2.0,

        escena->sceneRect().center().y()
            - limites.height() / 2.0
        );

    resultado->setZValue(100);

    qDebug() << mensaje;
}
