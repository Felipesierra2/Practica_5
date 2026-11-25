#ifndef REGISTROSIMULACION_H
#define REGISTROSIMULACION_H

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QPointF>

class RegistroSimulacion
{
public:
    RegistroSimulacion();
    ~RegistroSimulacion();

    bool abrirArchivo(const QString& nombreArchivo);
    void cerrarArchivo();

    bool estaAbierto() const;

    void registrarPosicion(
        qreal tiempo,
        int identificador,
        const QPointF& posicion
        );

    void registrarColision(
        qreal tiempo,
        int identificador,
        const QString& tipo,
        const QString& detalle
        );

    void registrarFusion(
        qreal tiempo,
        int identificador1,
        int identificador2,
        int identificadorFinal
        );

private:
    QFile archivo;
    QTextStream flujo;
};

#endif
