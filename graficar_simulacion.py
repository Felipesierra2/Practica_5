from pathlib import Path
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle

trayectorias = {}
colisiones = []
fusiones = []

ruta = Path(__file__).resolve().parent / "simulacion.txt"

with open(ruta, "r", encoding="utf-8") as archivo:

    for linea in archivo:

        datos = linea.strip().split(";")

        tipo = datos[0]

        if tipo == "POSICION":

            tiempo = float(datos[1])
            identificador = int(datos[2])
            x = float(datos[3])
            y = float(datos[4])

            if identificador not in trayectorias:
                trayectorias[identificador] = []

            trayectorias[identificador].append(
                (tiempo, x, y)
            )

        elif tipo == "COLISION":

            tiempo = float(datos[1])
            identificador = int(datos[2])
            estructura = datos[3]
            detalle = datos[4]

            colisiones.append(
                (tiempo, identificador, estructura, detalle)
            )

        elif tipo == "FUSION":

            tiempo = float(datos[1])
            identificador1 = int(datos[2])
            identificador2 = int(datos[3])
            identificadorFinal = int(datos[4])

            fusiones.append(
                (tiempo, identificador1,
                 identificador2, identificadorFinal)
            )

print("Particulas registradas:", len(trayectorias))

totalPosiciones = 0

for identificador, posiciones in trayectorias.items():

    print(
        "Particula:", identificador,
        "| Posiciones:", len(posiciones),
        "| Posicion inicial:", posiciones[0],
        "| Posicion final:", posiciones[-1]
    )

    totalPosiciones += len(posiciones)

print("Total de posiciones:", totalPosiciones)

print("\nTotal de colisiones:", len(colisiones))

print("\nPrimeras cinco colisiones:")

for colision in colisiones[:5]:
    print(colision)

print("\nTotal de fusiones:", len(fusiones))

print("\nFusiones registradas:")

for fusion in fusiones:
    print(fusion)

indicePosiciones = {}

for identificador, posiciones in trayectorias.items():

    for tiempo, x, y in posiciones:

        indicePosiciones[(tiempo, identificador)] = (x, y)

figura, ejes = plt.subplots(figsize=(15, 9))

ejes.set_xlim(0, 800)
ejes.set_ylim(600, 0)

ejes.set_aspect("equal")

ejes.set_title("Simulacion de particulas")
ejes.set_xlabel("Posicion X (px)")
ejes.set_ylabel("Posicion Y (px)")

ejes.set_facecolor("#1c202d")

paredes = [
    (0, 15, 15, 570),
    (785, 15, 15, 570),
    (0, 0, 800, 15),
    (0, 585, 800, 15)
]

for x, y, ancho, alto in paredes:

    pared = Rectangle(
        (x, y),
        ancho,
        alto,
        facecolor="#787878"
    )

    ejes.add_patch(pared)

obstaculos = [
    (250, 400, 80, 80, 0.8),
    (450, 250, 80, 80, 0.6),
    (600, 450, 80, 80, 0.4),
    (150, 150, 80, 80, 0.2)
]

for x, y, ancho, alto, coeficiente in obstaculos:

    obstaculo = Rectangle(
        (x, y),
        ancho,
        alto,
        facecolor="#a2784c"
    )

    ejes.add_patch(obstaculo)

    ejes.text(
        x + ancho / 2,
        y + alto / 2,
        f"e = {coeficiente}",
        color="white",
        ha="center",
        va="center"
    )

import numpy as np

colores = plt.get_cmap("turbo")(
    np.linspace(0.15, 0.85, len(trayectorias))
)

for indice, (identificador, posiciones) in enumerate(
    trayectorias.items()
):

    coordenadasX = []
    coordenadasY = []

    for tiempo, x, y in posiciones:

        coordenadasX.append(x)
        coordenadasY.append(y)

    ejes.plot(
        coordenadasX,
        coordenadasY,
        color=colores[indice],
        linewidth=1.2,
        label=f"Particula {identificador}"
    )

colisionesParedX = []
colisionesParedY = []

colisionesObstaculoX = []
colisionesObstaculoY = []

for tiempo, identificador, estructura, detalle in colisiones:

    clave = (tiempo, identificador)

    if clave not in indicePosiciones:
        continue

    x, y = indicePosiciones[clave]

    if estructura == "PARED":

        colisionesParedX.append(x)
        colisionesParedY.append(y)

    elif estructura == "OBSTACULO":

        colisionesObstaculoX.append(x)
        colisionesObstaculoY.append(y)

ejes.scatter(
    colisionesParedX,
    colisionesParedY,
    color="white",
    marker="x",
    s=30,
    linewidths=1.2,
    label="Colision elastica",
    zorder=5
)

ejes.scatter(
    colisionesObstaculoX,
    colisionesObstaculoY,
    color="#ff9800",
    marker="^",
    s=40,
    edgecolors="black",
    linewidths=0.5,
    label="Colision inelastica",
    zorder=5
)

fusionesX = []
fusionesY = []

for tiempo, id1, id2, idFinal in fusiones:

    clave = (tiempo, idFinal)

    if clave not in indicePosiciones:
        continue

    x, y = indicePosiciones[clave]

    fusionesX.append(x)
    fusionesY.append(y)

    ejes.annotate(
        f"{id1}+{id2}={idFinal}",
        (x, y),
        xytext=(5, -10),
        textcoords="offset points",
        color="#ffdf40",
        fontsize=8,
        zorder=7
    )

ejes.scatter(
    fusionesX,
    fusionesY,
    color="#ffdf40",
    marker="*",
    s=150,
    edgecolors="black",
    linewidths=0.8,
    label="Fusion de particulas",
    zorder=6
)

ejes.legend(
    loc="upper left",
    bbox_to_anchor=(1.02, 1),
    fontsize=8,
    ncol=2
)

plt.tight_layout()

rutaGrafica = ruta.parent / "trayectorias_simulacion.png"

figura.savefig(
    rutaGrafica,
    dpi=300,
    bbox_inches="tight"
)

print("\nGrafica guardada en:", rutaGrafica)

print("\nColisiones elasticas graficadas:",
      len(colisionesParedX))

print("Colisiones inelasticas graficadas:",
      len(colisionesObstaculoX))

print("Total de colisiones graficadas:",
      len(colisionesParedX) + len(colisionesObstaculoX))

print("\nFusiones graficadas:", len(fusionesX))

print("Fusiones sin posicion:",
      len(fusiones) - len(fusionesX))

plt.show()