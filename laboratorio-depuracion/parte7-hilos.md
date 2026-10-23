# Parte 7: análisis de hilos y condiciones de carrera

## Objetivo

Identificar una condición de carrera en un programa multihilo usando herramientas de análisis.

---

## Código base con condición de carrera

### Código original


```cpp
#include <iostream>
#include <thread>

int contador = 0;

void incrementar() {
    for (int i = 0; i < 100000; i++) {
        contador++;
    }
}

int main() {
    std::thread hilo1(incrementar);
    std::thread hilo2(incrementar);

    hilo1.join();
    hilo2.join();

    std::cout << "Contador final: " << contador << std::endl;

    return 0;
}
```

### Qué hace el programa

Define un contador global y una función `incrementar` que le suma 1 cien mil veces. En `main` se crean dos hilos que ejecutan esa función al mismo tiempo, se espera a que ambos terminen con `join` y se imprime el valor final.

### Resultado esperado del contador

Cada hilo hace 100 000 incrementos y hay dos hilos, por lo que el resultado correcto es 200 000, siempre.

### Comando de compilación

```bash
g++ -g -pthread -o race_condition codigo/08_race_condition.cpp
```

### Comando de ejecución

```bash
./race_condition
```

### Resultados obtenidos al ejecutar varias veces

Se ejecutó 8 veces.

```text
Contador final: 121671
Contador final: 101903
Contador final: 113211
Contador final: 106536
Contador final: 100000
Contador final: 100000
Contador final: 129262
Contador final: 124502
```

Ninguna de las ocho ejecuciones dio 200 000, y los valores cambian de una ejecución a otra. El programa compila sin advertencias y termina con normalidad, por lo que nada indica el problema aparte del resultado equivocado. Esa variación es la señal típica de una condición de carrera.

---

## Análisis con herramientas

### ThreadSanitizer

ThreadSanitizer (TSan) es una herramienta que detecta data races, y se activa al compilar con `-fsanitize=thread` [1].

```bash
g++ -g -fsanitize=thread -pthread -o race_condition_tsan codigo/08_race_condition.cpp
./race_condition_tsan
```

La primera ejecución no llegó a correr el programa.

```text
FATAL: ThreadSanitizer: unexpected memory mapping 0x6083a8214000-0x6083a8215000
```

Este error ocurre al iniciar, antes de ejecutar el código del programa, y probablemente se debe a la forma en que el kernel de WSL2 reparte las direcciones de memoria al azar (ASLR), ya que se resolvió desactivando esa aleatorización. Para ello se ejecutó el mismo binario con la aleatorización de direcciones desactivada solo para ese proceso, mediante `setarch -R`. No se modificó nada del sistema.

```bash
setarch -R ./race_condition_tsan
```

```text
==================
WARNING: ThreadSanitizer: data race (pid=19237)
  Read of size 4 at 0x555555559154 by thread T2:
    #0 incrementar() codigo/08_race_condition.cpp:8 (race_condition_tsan+0x13f9)
    #1 void std::__invoke_impl<void, void (*)()>(std::__invoke_other, void (*&&)()) /usr/include/c++/11/bits/invoke.h:61 (race_condition_tsan+0x228a)
    ...
    #6 <null> <null> (libstdc++.so.6+0xdc252)

  Previous write of size 4 at 0x555555559154 by thread T1:
    #0 incrementar() codigo/08_race_condition.cpp:8 (race_condition_tsan+0x1411)
    #1 void std::__invoke_impl<void, void (*)()>(std::__invoke_other, void (*&&)()) /usr/include/c++/11/bits/invoke.h:61 (race_condition_tsan+0x228a)
    ...
    #6 <null> <null> (libstdc++.so.6+0xdc252)

  Location is global 'contador' of size 4 at 0x555555559154 (race_condition_tsan+0x000000005154)

  Thread T2 (tid=19240, running) created by main thread at:
    #0 pthread_create ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:969 (libtsan.so.0+0x605b8)
    #1 std::thread::_M_start_thread(...) <null> (libstdc++.so.6+0xdc328)
    #2 main codigo/08_race_condition.cpp:14 (race_condition_tsan+0x1484)

  Thread T1 (tid=19239, finished) created by main thread at:
    #0 pthread_create ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:969 (libtsan.so.0+0x605b8)
    #1 std::thread::_M_start_thread(...) <null> (libstdc++.so.6+0xdc328)
    #2 main codigo/08_race_condition.cpp:13 (race_condition_tsan+0x146e)

SUMMARY: ThreadSanitizer: data race codigo/08_race_condition.cpp:8 in incrementar()
==================
Contador final: 200000
ThreadSanitizer: reported 1 warnings
```

Las líneas `...` reemplazan marcos internos de la biblioteca estándar que se repiten en ambos bloques.

| Dato del reporte | Qué indica |
|---|---|
| `data race` | Tipo de problema detectado. |
| `Read of size 4 ... by thread T2` | El hilo T2 leyó 4 bytes (el `int`) en la línea 8. |
| `Previous write of size 4 ... by thread T1` | El hilo T1 había escrito esa misma dirección, también en la línea 8, sin sincronización entre ambos. |
| `Location is global 'contador'` | La dirección compartida es la variable global `contador`. |
| `created by main thread at ... :13 / :14` | Los hilos se crearon en las líneas 13 y 14 de `main`. |
| `SUMMARY ... 08_race_condition.cpp:8 in incrementar()` | Resumen que ubica el problema en `contador++`, línea 8. |

Un detalle interesante es que en esta ejecución con TSan el contador final fue 200 000, o sea correcto, y aun así la herramienta reportó la carrera. Esto muestra que un resultado correcto no demuestra que el programa esté libre de carreras. TSan detecta el acceso sin sincronización aunque esa vez no haya producido un valor erróneo.

### Helgrind

Helgrind es una herramienta de Valgrind que detecta errores en programas con hilos POSIX, entre ellos las condiciones de carrera [2].

```bash
valgrind --tool=helgrind ./race_condition
```

```text
==19204== Helgrind, a thread error detector
==19204== Copyright (C) 2007-2017, and GNU GPL'd, by OpenWorks LLP et al.
==19204== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==19204== Command: ./race_condition
==19204==
==19204== ---Thread-Announcement------------------------------------------
==19204==
==19204== Thread #3 was created
==19204==    at 0x4BDAA83: clone (clone.S:76)
==19204==    ...
==19204==    by 0x109341: main (08_race_condition.cpp:14)
==19204==
==19204== ---Thread-Announcement------------------------------------------
==19204==
==19204== Thread #2 was created
==19204==    at 0x4BDAA83: clone (clone.S:76)
==19204==    ...
==19204==    by 0x10932B: main (08_race_condition.cpp:13)
==19204==
==19204== ----------------------------------------------------------------
==19204==
==19204== Possible data race during read of size 4 at 0x10C154 by thread #3
==19204== Locks held: none
==19204==    at 0x1092DA: incrementar() (08_race_condition.cpp:8)
==19204==    ...
==19204==
==19204== This conflicts with a previous write of size 4 by thread #2
==19204== Locks held: none
==19204==    at 0x1092E3: incrementar() (08_race_condition.cpp:8)
==19204==    ...
==19204==  Address 0x10c154 is 0 bytes inside data symbol "contador"
==19204==
==19204== ----------------------------------------------------------------
==19204==
==19204== Possible data race during write of size 4 at 0x10C154 by thread #3
==19204== Locks held: none
==19204==    at 0x1092E3: incrementar() (08_race_condition.cpp:8)
==19204==    ...
==19204==
==19204== This conflicts with a previous write of size 4 by thread #2
==19204== Locks held: none
==19204==    at 0x1092E3: incrementar() (08_race_condition.cpp:8)
==19204==    ...
==19204==  Address 0x10c154 is 0 bytes inside data symbol "contador"
==19204==
Contador final: 200000
==19204==
==19204== Use --history-level=approx or =none to gain increased speed, at
==19204== the cost of reduced accuracy of conflicting-access information
==19204== For lists of detected and suppressed errors, rerun with: -s
==19204== ERROR SUMMARY: 2 errors from 2 contexts (suppressed: 0 from 0)
```

Los `...` reemplazan los marcos internos de `std::thread` y `libstdc++`, que no aportan al análisis.

| Dato del reporte | Qué indica |
|---|---|
| `Possible data race during read of size 4 ... by thread #3` | Un hilo leyó `contador` en un momento en que otro hilo lo escribía. El texto dice "possible" porque Helgrind no observó ninguna relación de orden entre ambos accesos [2]. |
| `This conflicts with a previous write ... by thread #2` | Señala el acceso anterior, de otro hilo, con el que choca. |
| `Locks held: none` | Ninguno de los dos hilos tenía un candado tomado al acceder. |
| `incrementar() (08_race_condition.cpp:8)` | La línea del problema es `contador++`. |
| `0 bytes inside data symbol "contador"` | La dirección corresponde a la variable global `contador`. |
| `2 errors from 2 contexts` | Se reportaron dos carreras, una por la lectura y otra por la escritura de `contador++`. |

Ambas herramientas coinciden en lo esencial, que es el acceso concurrente a `contador` en la línea 8 sin ningún candado. Helgrind agrega la información explícita `Locks held: none`, que es una pista directa de la solución.

---

## Qué es una condición de carrera

Una condición de carrera ocurre cuando el resultado del programa depende del orden en que se intercalan los hilos. En C++, el caso concreto que se está viendo es una carrera de datos (data race). Sucede cuando dos hilos acceden a la misma posición de memoria, al menos uno de ellos escribe, los accesos no son atómicos y no hay nada que ordene un acceso respecto del otro [3]. Una carrera de datos es comportamiento indefinido [3].

En este programa, `contador++` no es una sola operación. Equivale a leer el valor, sumarle 1 y escribirlo de vuelta. Si ambos hilos leen el mismo valor antes de que alguno lo escriba, los dos escriben el mismo resultado y uno de los incrementos se pierde. Por eso el total queda por debajo de 200 000.

---

## Corrección con `std::mutex`

Se agregó un `std::mutex` global y, en cada iteración, un `std::lock_guard` que lo toma antes de ejecutar `contador++`. Mientras un hilo tiene el mutex, el otro debe esperar, por lo que el incremento se ejecuta por completo antes de que el otro hilo lo toque [4]. El `lock_guard` toma el mutex al crearse y lo libera automáticamente al salir de su ámbito, que aquí es el final de cada iteración [5].

### Código corregido

Archivo [`codigo/09_race_condition_corregido.cpp`](codigo/09_race_condition_corregido.cpp).

```cpp
#include <iostream>
#include <thread>
#include <mutex>

int contador = 0;
std::mutex mtx;

void incrementar() {
    for (int i = 0; i < 100000; i++) {
        std::lock_guard<std::mutex> lock(mtx);
        contador++;
    }
}

int main() {
    std::thread hilo1(incrementar);
    std::thread hilo2(incrementar);

    hilo1.join();
    hilo2.join();

    std::cout << "Contador final: " << contador << std::endl;

    return 0;
}
```

### Resultado después de corregir

```bash
g++ -g -pthread -o race_condition_corregido codigo/09_race_condition_corregido.cpp
./race_condition_corregido
```

Se ejecutó 5 veces y las cinco dieron el mismo resultado.

```text
Contador final: 200000
Contador final: 200000
Contador final: 200000
Contador final: 200000
Contador final: 200000
```

Con Helgrind:

```bash
valgrind --tool=helgrind ./race_condition_corregido
```

```text
==19302== Helgrind, a thread error detector
==19302== Copyright (C) 2007-2017, and GNU GPL'd, by OpenWorks LLP et al.
==19302== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==19302== Command: ./race_condition_corregido
==19302==
Contador final: 200000
==19302==
==19302== Use --history-level=approx or =none to gain increased speed, at
==19302== the cost of reduced accuracy of conflicting-access information
==19302== For lists of detected and suppressed errors, rerun with: -s
==19302== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 300004 from 7)
```

Con ThreadSanitizer:

```bash
g++ -g -fsanitize=thread -pthread -o race_condition_corregido_tsan codigo/09_race_condition_corregido.cpp
setarch -R ./race_condition_corregido_tsan
```

```text
Contador final: 200000
```

---

## Preguntas de reflexión

**1. ¿Por qué `contador++` no es una operación segura entre varios hilos?**

Porque no es una operación atómica. Se compone de tres pasos, que son leer el valor, sumarle 1 y guardar el resultado. Dos hilos pueden intercalar esos pasos. Si ambos leen el mismo valor antes de que alguno guarde, los dos guardan el mismo número y se pierde un incremento. Además, al ser una carrera de datos, el comportamiento del programa es indefinido [3].

**2. ¿Qué significa que dos hilos accedan a la misma variable compartida?**

Significa que ambos leen o escriben la misma posición de memoria. Los hilos de un proceso comparten el espacio de memoria, así que una variable global como `contador` es visible para los dos. Si solo leen, no hay problema. El riesgo aparece cuando al menos uno escribe y no existe sincronización entre ellos [3]. En el ejercicio, ambos hilos modifican `contador` en la línea 8.

**3. ¿Qué problema resuelve `std::mutex`?**

Resuelve el acceso simultáneo a datos. Solo un hilo puede tener el mutex a la vez, y los demás esperan hasta que se libere [4]. Así, la sección protegida, en este caso `contador++`, se ejecuta completa por un hilo antes de que entre otro, y se establece un orden entre los accesos que elimina la carrera.

**4. ¿Qué hace `std::lock_guard`?**

Es un envoltorio que toma el mutex cuando se crea y lo libera cuando se destruye, es decir, al salir de su ámbito [5]. Esto evita olvidar el `unlock` y garantiza que el mutex se libere aunque el código salga antes por una excepción. Por eso se prefiere sobre llamar a `lock` y `unlock` a mano [4]. En el código, la variable debe tener nombre (`lock`), ya que un temporal sin nombre se destruiría de inmediato y no protegería nada [5].

**5. ¿Por qué los errores de concurrencia pueden ser difíciles de reproducir?**

Porque dependen de cómo el planificador del sistema intercala los hilos, y eso cambia en cada ejecución. En las 8 ejecuciones sin corregir se obtuvieron 7 valores distintos. Además, observar el programa puede alterar su comportamiento. Con TSan y con Helgrind el resultado final fue 200 000, o sea correcto, aunque ambos reportaron la carrera. Un error que no aparece siempre puede pasar las pruebas y manifestarse después, en otro equipo o con otra carga.

**6. ¿Cuál herramienta le pareció más clara para analizar este caso: ThreadSanitizer o Helgrind?**

Para este caso resultó más clara la salida de ThreadSanitizer. Reporta una sola carrera con el hilo que lee, el hilo que escribió antes y el nombre de la variable (`Location is global 'contador'`), y el `SUMMARY` final apunta directo a la línea 8. Helgrind entrega más texto, con dos errores y marcos internos de `std::thread` que dificultan la lectura, aunque su línea `Locks held: none` orienta bien hacia la solución. Como contrapartida, TSan exigió recompilar el programa y en este entorno hubo que ejecutarlo con `setarch -R`, mientras que Helgrind trabajó directamente sobre el binario normal.[1].

---

## Referencias

[1] Clang Project, "ThreadSanitizer", en Clang Documentation. https://clang.llvm.org/docs/ThreadSanitizer.html

[2] Valgrind Developers, "Helgrind: a thread error detector", en Valgrind User Manual. https://valgrind.org/docs/manual/hg-manual.html

[3] cppreference, "Multi-threaded executions and data races", en C++ Language. https://en.cppreference.com/w/cpp/language/multithread

[4] cppreference, "std::mutex", en C++ Standard Library. https://en.cppreference.com/w/cpp/thread/mutex

[5] cppreference, "std::lock_guard", en C++ Standard Library. https://en.cppreference.com/w/cpp/thread/lock_guard
