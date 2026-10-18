# Laboratorio de programación concurrente y paralela en C++

## Estudiante

Nombre: Brandon Fuentes Jiménez

Carné: C12976

## Descripción

Este laboratorio explora conceptos básicos de programación concurrente y paralela usando C++. Se trabajan la creación de hilos con `std::thread`, las condiciones de carrera, la sincronización con `std::mutex` y `std::lock_guard`, la comparación de tiempos entre ejecución secuencial y paralela, el efecto de la cantidad de hilos sobre el rendimiento y un ejemplo de deadlock junto con su corrección usando `std::scoped_lock`.

Las respuestas a las preguntas de análisis de cada parte, junto con las tablas de resultados reales, están en [`resultados/analisis.md`](resultados/analisis.md).

## Requisitos

- g++ con soporte para C++17 o superior
- Sistema operativo Linux (el laboratorio se desarrolló y ejecutó en WSL)
- Soporte para hilos POSIX (`-pthread`)

## Estructura

```
laboratorio-concurrencia/
├── README.md
├── src/
│   ├── parte1_threads.cpp
│   ├── parte2_race_condition.cpp
│   ├── parte3_mutex.cpp
│   ├── parte4_benchmark.cpp
│   ├── parte5_hilos_vs_rendimiento.cpp
│   ├── parte6_deadlock.cpp
│   └── parte6_deadlock_corregido.cpp
└── resultados/
    └── analisis.md
```

## Compilación y ejecución

Todos los comandos se ejecutan desde la carpeta `laboratorio-concurrencia/`.

### Parte 1: creación básica de hilos

```bash
g++ -std=c++17 -Wall -Wextra -pthread src/parte1_threads.cpp -o parte1
./parte1
```

### Parte 2: condición de carrera

```bash
g++ -std=c++17 -Wall -Wextra -pthread src/parte2_race_condition.cpp -o parte2
./parte2
```

### Parte 3: corrección usando mutex

```bash
g++ -std=c++17 -Wall -Wextra -pthread src/parte3_mutex.cpp -o parte3
./parte3
```

### Parte 4: mini benchmark secuencial vs. paralelo

```bash
g++ -std=c++17 -O2 -Wall -Wextra -pthread src/parte4_benchmark.cpp -o parte4
./parte4
```

### Parte 5: cantidad de hilos vs. rendimiento

```bash
g++ -std=c++17 -O2 -Wall -Wextra -pthread src/parte5_hilos_vs_rendimiento.cpp -o parte5
./parte5
```

Para registrar los núcleos de la máquina se usaron `nproc` y `lscpu`.

### Parte 6: ejemplo simple de deadlock

Este programa se bloquea a propósito, por lo que se ejecuta con `timeout` para que no quede colgado:

```bash
g++ -std=c++17 -Wall -Wextra -pthread src/parte6_deadlock.cpp -o parte6
timeout 5 ./parte6
```

Si el programa no termina antes de los 5 segundos, `timeout` lo detiene con código de salida 124.

### Parte 6 (versión corregida)

```bash
g++ -std=c++17 -Wall -Wextra -pthread src/parte6_deadlock_corregido.cpp -o parte6_corregido
./parte6_corregido
```

Esta versión usa `std::scoped_lock` y sí termina normalmente.

Los ejecutables generados no se versionan, están listados en `.gitignore`.

## Conclusiones

- **Los hilos no garantizan orden.** En la Parte 1 el orden de los mensajes cambió de una ejecución a otra, y algunas líneas salieron mezcladas porque `std::cout` no estaba protegido. El orden de ejecución lo decide el planificador del sistema operativo, no el orden en que se crean los hilos.
- **Compartir datos sin sincronizar produce resultados incorrectos.** En la Parte 2 el contador esperado era 4000000, pero las cinco ejecuciones dieron valores distintos entre 1075562 y 1135545. Esto pasa porque `contador++` no es una sola operación y los hilos se pisan las escrituras.
- **El mutex corrige el problema, pero tiene un costo.** En la Parte 3 las cinco ejecuciones dieron 4000000. A cambio, los hilos se turnan para acceder a la sección crítica, y proteger cada incremento individual limita lo que se gana con el paralelismo.
- **Paralelizar ayuda cuando el trabajo se puede dividir.** En la Parte 4 cada hilo sumó su propio bloque del vector y guardó el resultado en su propia posición, sin necesidad de mutex. La versión paralela fue entre dos y tres veces más rápida que la secuencial en las cinco ejecuciones, y no llegó a ser 4 veces más rápida con 4 hilos por los costos de crear y esperar los hilos.
- **Más hilos no siempre es mejor.** En la Parte 5, en una máquina con 20 procesadores lógicos (`nproc`), el tiempo bajó de unos 30.3 ms con 1 hilo a unos 7.7 ms con 16 hilos. La mejora fue grande al pasar de 1 a 4 hilos y mucho menor al pasar de 8 a 16. Con 16 hilos se obtuvo el mejor tiempo medido en esta máquina, pero la ganancia adicional fue pequeña, así que el resultado depende del hardware y no es una regla general.
- **El deadlock se evita con buenas prácticas.** En la Parte 6 los dos hilos tomaban los recursos en orden contrario y el programa nunca terminó. La versión con `std::scoped_lock` tomó ambos mutex juntos y terminó en las cinco ejecuciones.
