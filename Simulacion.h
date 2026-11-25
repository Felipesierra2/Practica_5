#ifndef SIMULACION_H
#define SIMULACION_H

#include <QList>
#include <QRectF>
#include "particula.h"
#include "pared.h"
#include "obstaculo.h"
#include "registrosimulacion.h"

class Simulacion
{
public:
    Simulacion();
    ~Simulacion();

    Simulacion(const Simulacion&) = delete;
    Simulacion& operator=(const Simulacion&) = delete;

    void ejecutar();

private:
    QRectF limites;

    QList<Particula*> particulas;
    QList<Pared*> paredes;
    QList<Obstaculo*> obstaculos;

    RegistroSimulacion registro;

    qreal tiempoSimulacion;

    static constexpr qreal DELTA_TIEMPO = 0.016;
    static constexpr qreal DURACION = 15.0;

    void crearParedes();
    void crearObstaculos();
    void crearParticulasIniciales();

    void actualizarSimulacion(qreal deltaTiempo);

    void comprobarColisionesParedes();
    void comprobarColisionesObstaculos();
    void comprobarColisionesParticulas();

    void fusionarParticulas(int indice1, int indice2);

    void registrarPosiciones();
    void liberarMemoria();
};

#endif
