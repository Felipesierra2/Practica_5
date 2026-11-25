cmake_minimum_required(VERSION 3.16)

project(Practica_5 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(Practica_5
    main.cpp

    Particula.h
    Particula.cpp

    Pared.h
    Pared.cpp

    Obstaculo.h
    Obstaculo.cpp

    RegistroSimulacion.h
    RegistroSimulacion.cpp

    Simulacion.h
    Simulacion.cpp
)

target_link_libraries(Practica_5 PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
)
