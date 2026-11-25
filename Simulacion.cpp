#include "simulacion.h"
#include <QtMath>
#include <QDebug>
#include <QDir>

Simulacion::Simulacion()
    : limites(0, 0, 800, 600),
    tiempoSimulacion(0.0)
{
    crearParedes();
    crearObstaculos();
    crearParticulasIniciales();
}

void Simulacion::liberarMemoria()
{
    for (Particula* particula : particulas)
        delete particula;

    particulas.clear();

    for (Pared* pared : paredes)
        delete pared;

    paredes.clear();

    for (Obstaculo* obstaculo : obstaculos)
        delete obstaculo;

    obstaculos.clear();
}

Simulacion::~Simulacion()
{
    liberarMemoria();
}

void Simulacion::crearParedes()
{
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
}

void Simulacion::crearObstaculos()
{
    obstaculos.append(new Obstaculo(
        QRectF(250, 400, 80, 80), 0.8));

    obstaculos.append(new Obstaculo(
        QRectF(450, 250, 80, 80), 0.6));

    obstaculos.append(new Obstaculo(
        QRectF(600, 450, 80, 80), 0.4));

    obstaculos.append(new Obstaculo(
        QRectF(150, 150, 80, 80), 0.2));
}

void Simulacion::crearParticulasIniciales()
{
    particulas.append(new Particula(
        1.0, 10.0,
        QPointF(80, 500),
        QPointF(15, -18)
        ));

    particulas.append(new Particula(
        2.0, 12.0,
        QPointF(700, 350),
        QPointF(-12, -8)
        ));

    particulas.append(new Particula(
        1.5, 11.0,
        QPointF(350, 100),
        QPointF(8, 5)
        ));

    particulas.append(new Particula(
        3.0, 14.0,
        QPointF(400, 500),
        QPointF(-10, -15)
        ));
}

void Simulacion::ejecutar()
{
    if (!registro.abrirArchivo("simulacion.txt"))
    {
        qDebug() << "No se pudo iniciar el registro.";
        return;
    }

    qDebug() << "Iniciando simulacion...";
    qDebug() << "Particulas iniciales:" << particulas.size();

    tiempoSimulacion = 0.0;

    registrarPosiciones();

    while (tiempoSimulacion < DURACION)
    {
        const qreal deltaTiempo = qMin(
            DELTA_TIEMPO,
            DURACION - tiempoSimulacion
            );

        actualizarSimulacion(deltaTiempo);
    }

    registro.cerrarArchivo();

    qDebug() << "Simulacion finalizada.";
    qDebug() << "Tiempo simulado:" << tiempoSimulacion << "s";
    qDebug() << "Particulas restantes:" << particulas.size();

    qDebug() << "Archivo generado:"
             << QDir::current().absoluteFilePath("simulacion.txt");
}

void Simulacion::actualizarSimulacion(qreal deltaTiempo)
{
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

void Simulacion::comprobarColisionesParedes()
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
                         << energiaFinal - energiaInicial << "J";
            }
        }
    }
}

void Simulacion::comprobarColisionesObstaculos()
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
                        obstaculo->obtenerCoeficienteRestitucion())
                    );

                qDebug() << "Colision con obstaculo.";

                qDebug() << "Coeficiente:"
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

void Simulacion::comprobarColisionesParticulas()
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

void Simulacion::fusionarParticulas(int indice1, int indice2)
{
    Particula* primera = particulas.at(indice1);
    Particula* segunda = particulas.at(indice2);

    registro.registrarPosicion(
        tiempoSimulacion,
        primera->obtenerIdentificador(),
        primera->pos()
        );

    registro.registrarPosicion(
        tiempoSimulacion,
        segunda->obtenerIdentificador(),
        segunda->pos()
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
        (primera->pos() * masa1 +
         segunda->pos() * masa2) / masaFinal;

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

    particulas.removeAt(indice2);
    particulas.removeAt(indice1);

    delete primera;
    delete segunda;

    particulas.append(fusionada);

    qDebug() << "========== FUSION DE PARTICULAS ==========";
    qDebug() << "Masa resultante:" << masaFinal << "kg";
    qDebug() << "Radio resultante:" << radioFinal << "px";
    qDebug() << "Velocidad resultante:" << velocidadFinal << "m/s";
    qDebug() << "Momento lineal inicial:" << momentoInicial;
    qDebug() << "Momento lineal final:" << momentoFinal;
    qDebug() << "Diferencia de momento:" << diferenciaMomento;
    qDebug() << "Energia inicial:" << energiaInicial << "J";
    qDebug() << "Energia final:" << energiaFinal << "J";
    qDebug() << "Energia perdida:" << energiaPerdida << "J";
    qDebug() << "Particulas restantes:" << particulas.size();
    qDebug() << "=========================================";
}

void Simulacion::registrarPosiciones()
{
    for (Particula* particula : particulas)
    {
        if (!particula->estaLanzada())
            continue;

        registro.registrarPosicion(
            tiempoSimulacion,
            particula->obtenerIdentificador(),
            particula->pos()
            );
    }
}
