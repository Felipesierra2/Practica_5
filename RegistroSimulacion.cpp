#include "registrosimulacion.h"
#include <QDebug>

RegistroSimulacion::RegistroSimulacion()
{
}

bool RegistroSimulacion::abrirArchivo(
    const QString& nombreArchivo)
{
    if (archivo.isOpen())
        cerrarArchivo();

    archivo.setFileName(nombreArchivo);

    if (!archivo.open(QIODevice::WriteOnly |
                      QIODevice::Text |
                      QIODevice::Truncate))
    {
        qDebug() << "Error al abrir el archivo:"
                 << archivo.errorString();

        return false;
    }

    flujo.setDevice(&archivo);

    qDebug() << "Archivo de simulacion abierto correctamente.";

    return true;
}

bool RegistroSimulacion::estaAbierto() const
{
    return archivo.isOpen();
}

void RegistroSimulacion::cerrarArchivo()
{
    if (!archivo.isOpen())
        return;

    flujo.flush();

    flujo.setDevice(nullptr);

    archivo.close();

    qDebug() << "Archivo de simulacion cerrado correctamente.";
}

RegistroSimulacion::~RegistroSimulacion()
{
    cerrarArchivo();
}

void RegistroSimulacion::registrarPosicion(
    qreal tiempo,
    int identificador,
    const QPointF& posicion)
{
    if (!estaAbierto())
        return;

    flujo << "POSICION" << ";"
          << tiempo << ";"
          << identificador << ";"
          << posicion.x() << ";"
          << posicion.y() << "\n";
}

void RegistroSimulacion::registrarColision(
    qreal tiempo,
    int identificador,
    const QString& tipo,
    const QString& detalle)
{
    if (!estaAbierto())
        return;

    flujo << "COLISION" << ";"
          << tiempo << ";"
          << identificador << ";"
          << tipo << ";"
          << detalle << "\n";
}

void RegistroSimulacion::registrarFusion(
    qreal tiempo,
    int identificador1,
    int identificador2,
    int identificadorFinal)
{
    if (!estaAbierto())
        return;

    flujo << "FUSION" << ";"
          << tiempo << ";"
          << identificador1 << ";"
          << identificador2 << ";"
          << identificadorFinal << "\n";
}
