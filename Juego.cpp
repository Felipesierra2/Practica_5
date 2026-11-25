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
    partidaFinalizada(false)
{
    inicializarEscenario();
    crearJugadores();
    crearParedes();
    crearObstaculos();
    crearIndicadores();
    crearHUD();

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

Particula* Juego::crearParticula()
{
    if (!jugadorActual)
        return nullptr;

    QPointF posicionInicial =
        jugadorActual->obtenerPosicionLanzamiento();

    Particula* nuevaParticula = new Particula(
        1.0,
        10.0,
        posicionInicial
        );

    escena->addItem(nuevaParticula);

    particulas.append(nuevaParticula);

    return nuevaParticula;
}

void Juego::keyPressEvent(QKeyEvent* evento)
{
    if (partidaFinalizada)
    {
        if (evento->key() == Qt::Key_R &&
            !evento->isAutoRepeat())
        {
            reiniciarPartida();
        }

        return;
    }

    if (!jugadorActual)
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

    for (Particula* particula : particulas)
    {
        particula->actualizarMovimiento(deltaTiempo);
    }

    comprobarColisionesParedes();

    comprobarColisionesObstaculos();

    comprobarColisionesJugadores();

    eliminarParticulasFueraEscenario();

    comprobarFinPartida();
}

void Juego::dispararParticula()
{
    if (partidaFinalizada || !jugadorActual)
        return;

    Particula* nuevaParticula = crearParticula();

    if (!nuevaParticula)
        return;

    nuevaParticula->establecerPropietario(
        jugadorActual == jugador1 ? 1 : 2
        );

    nuevaParticula->lanzar(
        jugadorActual->obtenerVelocidadLanzamiento()
        );

    cambiarTurno();
}

void Juego::crearIndicadores()
{
    indicadorLanzamiento = escena->addLine(
        QLineF(),
        QPen(Qt::green, 2)
        );

    actualizarIndicadores();
}

void Juego::actualizarIndicadores()
{
    if (!jugadorActual)
        return;

    QPointF origen = jugadorActual->obtenerPosicionLanzamiento();

    QVector2D velocidad = jugadorActual->obtenerVelocidadLanzamiento();

    QPointF extremo = origen + (velocidad * 4.0f).toPointF();

    indicadorLanzamiento->setLine(QLineF(origen, extremo));

    if (hud)
    {
        hud->actualizarTurno(jugadorActual == jugador1 ? 1 : 2);

        hud->actualizarAngulo(jugadorActual->obtenerAngulo());

        hud->actualizarRapidez(jugadorActual->obtenerRapidez());
    }
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
    // Fortaleza del jugador 1

    obstaculos.append(new Obstaculo(
        QRectF(100, 465, 30, 120), 0.6, 200.0, 1
        ));

    obstaculos.append(new Obstaculo(
        QRectF(230, 465, 30, 120), 0.6, 200.0, 1
        ));

    obstaculos.append(new Obstaculo(
        QRectF(100, 435, 160, 30), 0.4, 100.0, 1
        ));

    // Fortaleza del jugador 2

    obstaculos.append(new Obstaculo(
        QRectF(540, 465, 30, 120), 0.6, 200.0, 2
        ));

    obstaculos.append(new Obstaculo(
        QRectF(670, 465, 30, 120), 0.6, 200.0, 2
        ));

    obstaculos.append(new Obstaculo(
        QRectF(540, 435, 160, 30), 0.4, 100.0, 2
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

                const QPointF momentoInicial =
                    particula->calcularMomentoLineal();

                obstaculo->resolverColision(particula);

                const QPointF velocidadFinal =
                    particula->obtenerVelocidad();

                qreal energiaFinal =
                    particula->calcularEnergiaCinetica();

                if (velocidadInicial == velocidadFinal)
                    continue;

                const qreal momentoImpacto = qSqrt(
                    momentoInicial.x() * momentoInicial.x() +
                    momentoInicial.y() * momentoInicial.y()
                    );

                const qreal dano =
                    FACTOR_DANO_ESTRUCTURAL * momentoImpacto;

                if (particula->obtenerPropietario() !=
                    obstaculo->obtenerPropietario())
                {
                    obstaculo->recibirDano(dano);
                }

                qDebug() << "Momento lineal de impacto:"
                         << momentoImpacto << "kg*m/s";

                qDebug() << "Dano estructural:" << dano;

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

void Juego::cambiarTurno()
{
    if (jugadorActual == jugador1)
        jugadorActual = jugador2;
    else
        jugadorActual = jugador1;

    actualizarIndicadores();
    actualizarIndicadorTurno();
}

void Juego::crearJugadores()
{
    jugador1 = new Jugador(true, QColor(37, 99, 235));
    jugador2 = new Jugador(false, QColor(220, 38, 38));

    jugador1->establecerNombre("MR.OCTOBER");
    jugador2->establecerNombre("Jugador 2");

    QRectF limites = escena->sceneRect();

    jugador1->setPos(
        limites.left() + 60,
        limites.top() + 300
    );

    jugador2->setPos(
        limites.right() - 60,
        limites.top() + 300
    );

    escena->addItem(jugador1);
    escena->addItem(jugador2);

    jugador1->establecerPosicionRepresentante(
        QPointF(180, 585)
        );

    jugador2->establecerPosicionRepresentante(
        QPointF(620, 585)
        );

    jugadorActual = jugador1;
    actualizarIndicadorTurno();
}

void Juego::comprobarColisionesJugadores()
{
    for (int i = particulas.size() - 1; i >= 0; i--)
    {
        Particula* particula = particulas.at(i);

        if (!particula->estaLanzada())
            continue;

        Jugador* jugadorImpactado = nullptr;

        if (particula->obtenerPropietario() != 1 &&
            jugador1->detectarImpacto(particula))
        {
            jugadorImpactado = jugador1;
        }
        else if (particula->obtenerPropietario() != 2 &&
                 jugador2->detectarImpacto(particula))
        {
            jugadorImpactado = jugador2;
        }

        if (!jugadorImpactado)
            continue;

        const qreal energia =
            particula->calcularEnergiaCinetica();

        const qreal dano = energia * FACTOR_DANO;

        jugadorImpactado->recibirDano(dano);

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
    actualizarIndicadorTurno();

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

    mensajeResultado = escena->addText(mensaje);

    mensajeResultado->setFont(
        QFont("Arial", 28, QFont::Bold)
        );

    mensajeResultado->setDefaultTextColor(Qt::yellow);

    QRectF limites = mensajeResultado->boundingRect();

    mensajeResultado->setPos(
        escena->sceneRect().center().x()
            - limites.width() / 2.0,

        escena->sceneRect().center().y()
            - limites.height() / 2.0
        );

    mensajeResultado->setZValue(100);

    mensaje += "\nPresiona R para reiniciar";

    qDebug() << mensaje;
}

void Juego::reiniciarPartida()
{
    if (!partidaFinalizada)
        return;

    temporizador->stop();

    // Eliminar las particulas de la partida anterior
    for (Particula* particula : particulas)
    {
        escena->removeItem(particula);
        delete particula;
    }

    particulas.clear();

    // Eliminar el mensaje de victoria
    if (mensajeResultado)
    {
        escena->removeItem(mensajeResultado);

        delete mensajeResultado;

        mensajeResultado = nullptr;
    }

    // Restablecer los jugadores
    jugador1->reiniciar();
    jugador2->reiniciar();

    for (Obstaculo* obstaculo : obstaculos)
    {
        obstaculo->reiniciar();
    }

    // Restablecer el estado de la partida
    jugadorActual = jugador1;

    partidaFinalizada = false;

    actualizarIndicadorTurno();

    // Restablecer los indicadores
    indicadorLanzamiento->setVisible(true);

    actualizarIndicadores();

    // Reactivar la simulacion
    reloj.restart();

    temporizador->start(16);

    qDebug() << "Nueva partida iniciada.";
}

void Juego::actualizarIndicadorTurno()
{
    if (!jugador1 || !jugador2)
        return;

    jugador1->establecerTurnoActivo(
        !partidaFinalizada && jugadorActual == jugador1
        );

    jugador2->establecerTurnoActivo(
        !partidaFinalizada && jugadorActual == jugador2
        );
}

void Juego::crearHUD()
{
    hud = new HUD();

    escena->addItem(hud);

    const QRectF limites = escena->sceneRect();

    qreal x = limites.center().x()
              - hud->boundingRect().center().x();

    qreal y = limites.top() + 20;

    hud->setPos(x, y);

    actualizarIndicadores();
}
