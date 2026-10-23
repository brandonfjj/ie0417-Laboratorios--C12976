# Parte 8: mini reto de depuración y Parte 9: reflexión final

## Mini reto de depuración

### Objetivo

Aplicar lo aprendido en un programa con varios errores pequeños.

---

### Código original

Archivo [`codigo/10_reto.cpp`](codigo/10_reto.cpp). Aquí se muestra la versión original, con los errores. En el repositorio el `.cpp` quedó en su versión corregida.

```cpp
#include <iostream>

int obtener_mayor(int* datos, int tamano) {
    int mayor = 0;

    for (int i = 0; i <= tamano; i++) {
        if (datos[i] > mayor) {
            mayor = datos[i];
        }
    }

    return mayor;
}

double calcular_promedio(int* datos, int tamano) {
    int suma = 0;

    for (int i = 0; i < tamano; i++) {
        suma += datos[i];
    }

    return suma;
}

int main() {
    int tamano = 5;
    int* datos = new int[tamano];

    datos[0] = -10;
    datos[1] = -20;
    datos[2] = -5;
    datos[3] = -30;
    datos[4] = -15;

    int mayor = obtener_mayor(datos, tamano);
    double promedio = calcular_promedio(datos, tamano);

    std::cout << "Mayor: " << mayor << std::endl;
    std::cout << "Promedio: " << promedio << std::endl;

    return 0;
}
```

### Qué hace el programa

Reserva un arreglo dinámico de 5 enteros, todos negativos, y calcula el mayor de ellos y el promedio mediante dos funciones. Luego imprime ambos valores. A mano, el mayor es -5 y el promedio es (-10 - 20 - 5 - 30 - 15) / 5 = -80 / 5 = -16.

### Resultado inicial del programa

```bash
g++ -g -o reto codigo/10_reto.cpp
./reto
```

```text
Mayor: 0
Promedio: -80
```

El programa compila sin errores y termina con normalidad, pero los dos resultados son incorrectos. El mayor no puede ser 0 porque ningún dato es 0, y el promedio de números que van de -30 a -5 no puede ser -80. Los otros dos problemas, el acceso fuera de límites y la fuga de memoria, no producen ningún síntoma visible en la ejecución normal.

### Herramientas usadas

Se usaron cuatro herramientas, la ejecución normal para ver el síntoma, Valgrind para la fuga y el acceso inválido, AddressSanitizer para confirmar el acceso inválido, y `gdb` para observar el valor de las variables dentro de las funciones. El enunciado pedía al menos dos. Valgrind y AddressSanitizer se explican en [1] y [2], y los comandos de `gdb` usados en [3].

---

### Errores encontrados y cómo se encontró cada uno

#### Error 1: acceso fuera de límites en `obtener_mayor`

**Valgrind.**

```bash
valgrind --leak-check=full ./reto
```

```text
==1101== Memcheck, a memory error detector
==1101== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==1101== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==1101== Command: ./reto
==1101== 
==1101== Invalid read of size 4
==1101==    at 0x10925C: obtener_mayor(int*, int) (10_reto.cpp:7)
==1101==    by 0x109376: main (10_reto.cpp:35)
==1101==  Address 0x4dd4c94 is 0 bytes after a block of size 20 alloc'd
==1101==    at 0x484A2F3: operator new[](unsigned long) (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==1101==    by 0x10931F: main (10_reto.cpp:27)
==1101== 
Mayor: 0
Promedio: -80
==1101== 
==1101== HEAP SUMMARY:
==1101==     in use at exit: 20 bytes in 1 blocks
==1101==   total heap usage: 3 allocs, 2 frees, 76,820 bytes allocated
==1101== 
==1101== 20 bytes in 1 blocks are definitely lost in loss record 1 of 1
==1101==    at 0x484A2F3: operator new[](unsigned long) (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==1101==    by 0x10931F: main (10_reto.cpp:27)
==1101== 
==1101== LEAK SUMMARY:
==1101==    definitely lost: 20 bytes in 1 blocks
==1101==    indirectly lost: 0 bytes in 0 blocks
==1101==    possibly lost: 0 bytes in 0 blocks
==1101==    still reachable: 0 bytes in 0 blocks
==1101==         suppressed: 0 bytes in 0 blocks
==1101== 
==1101== For lists of detected and suppressed errors, rerun with: -s
==1101== ERROR SUMMARY: 2 errors from 2 contexts (suppressed: 0 from 0)
```

El primer reporte, `Invalid read of size 4`, indica que se leyó un `int` en la línea 7, dentro de `obtener_mayor`. La frase `0 bytes after a block of size 20` dice que la dirección está justo después del bloque de 20 bytes (5 enteros) reservado en la línea 27. Es decir, se leyó `datos[5]`, que no existe [1].

**AddressSanitizer.**

```bash
g++ -g -fsanitize=address -o reto_asan codigo/10_reto.cpp
./reto_asan
```

```text
=================================================================
==1107==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x503000000054 at pc 0x60dc68e513bb bp 0x7fff3a1d5580 sp 0x7fff3a1d5570
READ of size 4 at 0x503000000054 thread T0
    #0 0x60dc68e513ba in obtener_mayor(int*, int) codigo/10_reto.cpp:7
    #1 0x60dc68e51665 in main codigo/10_reto.cpp:35
    #2 0x776566429d8f in __libc_start_call_main ../sysdeps/nptl/libc_start_call_main.h:58
    #3 0x776566429e3f in __libc_start_main_impl ../csu/libc-start.c:392
    #4 0x60dc68e51284 in _start (/home/brandonfj/repos/ie0417-laboratorios-C12976/laboratorio-depuracion/reto_asan+0x1284)

0x503000000054 is located 0 bytes to the right of 20-byte region [0x503000000040,0x503000000054)
allocated by thread T0 here:
    #0 0x776566cb6357 in operator new[](unsigned long) ../../../../src/libsanitizer/asan/asan_new_delete.cpp:102
    #1 0x60dc68e514ef in main codigo/10_reto.cpp:27
    #2 0x776566429d8f in __libc_start_call_main ../sysdeps/nptl/libc_start_call_main.h:58

SUMMARY: AddressSanitizer: heap-buffer-overflow codigo/10_reto.cpp:7 in obtener_mayor(int*, int)
==1107==ABORTING
```

El tipo de error es `heap-buffer-overflow`, una lectura de 4 bytes en la línea 7, justo a la derecha del bloque de 20 bytes. Ambas herramientas coinciden en la línea y en el tamaño del bloque. ASan lo nombra con más precisión y Valgrind lo deja como `Invalid read`.

**gdb.** Se siguió la secuencia sugerida por el enunciado.

```bash
gdb ./reto
```

```text
Breakpoint 1 at 0x12f2: file codigo/10_reto.cpp, line 26.
Breakpoint 1, main () at codigo/10_reto.cpp:26
26	    int tamano = 5;
27	    int* datos = new int[tamano];
29	    datos[0] = -10;
Breakpoint 2 at 0x555555555238: file codigo/10_reto.cpp, line 4.
Breakpoint 2, obtener_mayor (datos=0x55555556aeb0, tamano=5) at codigo/10_reto.cpp:4
4	    int mayor = 0;
$1 = 5
6	    for (int i = 0; i <= tamano; i++) {
$2 = 32767
Cannot access memory at address 0x55555558aeac
```

Con `break main`, `run` y dos `next` se llegó a la línea 29 y se puso el segundo breakpoint en `obtener_mayor`. Ahí `print tamano` mostró 5. Después de un `next` el programa quedó en la línea 6, antes de ejecutar la inicialización del `for`, por lo que `i` todavía tenía basura. Con ese valor, `print datos[i]` intentó leer una dirección lejana y gdb respondió `Cannot access memory`. No es un error del programa sino de haber consultado `i` antes de tiempo. Por eso se repitió la depuración avanzando un `next` más y con un breakpoint condicional en el índice problemático [4].

```text
Breakpoint 1, obtener_mayor (datos=0x55555556aeb0, tamano=5) at codigo/10_reto.cpp:4
4	    int mayor = 0;
6	    for (int i = 0; i <= tamano; i++) {
7	        if (datos[i] > mayor) {
$1 = 0
$2 = -10
$3 = 0
Breakpoint 2, obtener_mayor (datos=0x55555556aeb0, tamano=5) at codigo/10_reto.cpp:7
7	        if (datos[i] > mayor) {
$4 = 5
$5 = 0
$6 = 0
#0  obtener_mayor (datos=0x55555556aeb0, tamano=5) at codigo/10_reto.cpp:7
#1  0x0000555555555377 in main () at codigo/10_reto.cpp:35
```

En la primera iteración `i` vale 0, `datos[i]` vale -10 y `mayor` vale 0. Ahí ya se nota el error 2. El breakpoint condicional `break 7 if i == 5` se detuvo cuando `i` llegó a 5, que es un índice inválido para un arreglo de 5 elementos. El valor `datos[5]` fue 0, o sea basura que casualmente era cero. El `backtrace` confirma que la llamada viene de la línea 35 de `main`.

La causa es la condición `i <= tamano` del `for`. Los índices válidos van de 0 a `tamano - 1`, y con `<=` se hace una iteración de más. Acceder fuera de los límites de un arreglo es comportamiento indefinido [5].

#### Error 2: error lógico en el cálculo del mayor

**Cómo se encontró.** Se notó en la ejecución normal, porque el resultado fue `Mayor: 0` y 0 no está entre los datos. En `gdb` se vio en la primera iteración que `mayor` empieza en 0 y que `datos[0]` vale -10, que no es mayor que 0, por lo que `mayor` nunca se actualiza. Como todos los datos son negativos, ninguno supera al valor inicial.

Nótese que el acceso fuera de límites del error 1 no influyó en este resultado, ya que `datos[5]` valía 0 y 0 no es mayor que 0. El valor de `mayor` es incorrecto por la inicialización, no por la lectura de más.

#### Error 3: promedio mal calculado en `calcular_promedio`

**Cómo se encontró.** En la ejecución normal el promedio fue -80, igual a la suma de los datos, y el promedio a mano es -16. Para confirmarlo se usó `gdb` con un breakpoint en la línea del `return`.

```bash
gdb ./reto
(gdb) break 22
(gdb) run
(gdb) print suma
(gdb) print tamano
(gdb) print suma / tamano
(gdb) finish
```

```text
Breakpoint 1, calcular_promedio (datos=0x55555556aeb0, tamano=5) at codigo/10_reto.cpp:22
22	    return suma;
$1 = -80
$2 = 5
$3 = -16
0x000055555555538b in main () at codigo/10_reto.cpp:36
36	    double promedio = calcular_promedio(datos, tamano);
Value returned is $4 = -80
```

La suma es -80 y el tamaño es 5. La expresión `suma / tamano` da -16, que es el valor correcto, pero la función devuelve `suma` directamente. Es el mismo olvido que en el ejercicio de la Parte 2, no dividir entre la cantidad de datos. En este caso hay un cuidado adicional. Al dividir dos `int`, C++ trunca el resultado [6], por lo que un promedio como -16.4 saldría como -16. Por eso conviene convertir la suma a `double` antes de dividir.

#### Error 4: pérdida de memoria en `main`

**Cómo se encontró.** La ejecución normal no muestra nada. Valgrind lo reportó con `20 bytes in 1 blocks are definitely lost`, y en el resumen del heap `3 allocs, 2 frees`. Hubo una reserva sin su liberación. La traza apunta a la línea 27 de `main`, donde se hizo `new int[tamano]`. Ese arreglo nunca se liberó [1].

ASan no reportó la fuga en esta corrida, porque abortó al encontrar el acceso fuera de límites, antes de terminar el programa. Con el programa corregido en el error 1 la fuga se podría ver, pero Valgrind ya la había mostrado con claridad.

---

### Corrección realizada

Se hicieron cuatro cambios, uno por error.

| Error | Corrección |
|---|---|
| Acceso fuera de límites | La condición del `for` pasó de `i <= tamano` a `i < tamano`. |
| Mayor incorrecto con negativos | `mayor` se inicializa con `datos[0]` en lugar de 0, y el ciclo empieza en `i = 1`. |
| Promedio incorrecto | Se devuelve `static_cast<double>(suma) / tamano`. |
| Fuga de memoria | Se agregó `delete[] datos;` antes del `return` de `main`. Como se reservó con `new[]`, se libera con `delete[]` [7]. |

### Código corregido

Archivo [`codigo/10_reto.cpp`](codigo/10_reto.cpp).

```cpp
#include <iostream>

int obtener_mayor(int* datos, int tamano) {
    // Se parte del primer elemento, no de 0, para que funcione con negativos
    int mayor = datos[0];

    // El índice llega hasta tamano - 1, así no se sale del arreglo
    for (int i = 1; i < tamano; i++) {
        if (datos[i] > mayor) {
            mayor = datos[i];
        }
    }

    return mayor;
}

double calcular_promedio(int* datos, int tamano) {
    int suma = 0;

    for (int i = 0; i < tamano; i++) {
        suma += datos[i];
    }

    // Se divide entre la cantidad de datos, convirtiendo a double para no truncar
    return static_cast<double>(suma) / tamano;
}

int main() {
    int tamano = 5;
    int* datos = new int[tamano];

    datos[0] = -10;
    datos[1] = -20;
    datos[2] = -5;
    datos[3] = -30;
    datos[4] = -15;

    int mayor = obtener_mayor(datos, tamano);
    double promedio = calcular_promedio(datos, tamano);

    std::cout << "Mayor: " << mayor << std::endl;
    std::cout << "Promedio: " << promedio << std::endl;

    // Se libera el arreglo reservado con new[]
    delete[] datos;

    return 0;
}
```

Esta versión asume que `tamano` es mayor que 0. Con un arreglo vacío, `datos[0]` sería un acceso inválido. Para este reto, donde el tamaño es fijo, se consideró suficiente.

---

### Resultado final

```bash
g++ -g -Wall -Wextra -o reto codigo/10_reto.cpp
./reto
```

```text
Mayor: -5
Promedio: -16
```

Compiló sin advertencias, y los dos valores coinciden con el cálculo a mano. Con Valgrind:

```bash
valgrind --leak-check=full ./reto
```

```text
==1224== Memcheck, a memory error detector
==1224== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==1224== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==1224== Command: ./reto
==1224== 
Mayor: -5
Promedio: -16
==1224== 
==1224== HEAP SUMMARY:
==1224==     in use at exit: 0 bytes in 0 blocks
==1224==   total heap usage: 3 allocs, 3 frees, 76,820 bytes allocated
==1224== 
==1224== All heap blocks were freed -- no leaks are possible
==1224== 
==1224== For lists of detected and suppressed errors, rerun with: -s
==1224== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

Con AddressSanitizer:

```bash
g++ -g -fsanitize=address -o reto_asan codigo/10_reto.cpp
./reto_asan
```

```text
Mayor: -5
Promedio: -16
```

Valgrind informa 0 errores y todas las reservas liberadas (3 `allocs` y 3 `frees`). ASan no emite ningún reporte y el programa termina con código de salida 0.

### Qué herramienta fue más útil y por qué

Para este reto la más útil fue Valgrind, porque con una sola ejecución encontró dos de los cuatro problemas, el acceso inválido y la fuga, con la línea exacta de cada uno. ASan describió mejor el acceso fuera de límites, pero se detuvo ahí y no mostró la fuga. `gdb` fue lo que permitió entender los dos errores lógicos, ya que ninguna herramienta de memoria los puede detectar. Allí hizo falta ver los valores de `mayor`, `suma` y `tamano` mientras corría el programa. En resumen, Valgrind o ASan sirven para los errores de memoria y `gdb` para los de lógica, y en este reto hicieron falta ambos tipos.

### Reflexión del ejercicio

Lo que más llamó la atención fue que el programa tenía cuatro errores y solo dos se notaban en la salida. El acceso fuera de límites y la fuga no daban ninguna señal, y el programa parecía terminar bien. Además, un error puede ocultar a otro. Si se hubiera corregido solo el `for`, el mayor seguiría siendo 0 y habría parecido que aún faltaba algo en el acceso. Revisar cada herramienta por separado y corregir un error a la vez, verificando después, evitó esa confusión.

---

# Parte 9: reflexión final

## Reflexión final

**1. ¿Qué entiende ahora por depuración?**

Es el proceso de encontrar la causa de un comportamiento incorrecto y corregirla, poe medio de la evidencia y no en suposiciones. No se trata de cambiar el código hasta que funcione. El proceso que se siguió en el laboratorio fue ejecutar, observar el síntoma, reproducirlo, usar una herramienta, identificar la causa, corregir y verificar que el problema desapareció. Cada herramienta ayuda a una parte de ese proceso.

**2. ¿Por qué no basta con que un programa compile?**

Porque el compilador solo verifica que el código respete las reglas del lenguaje, no que haga lo que se quería. Varios programas del laboratorio compilaban sin advertencias y aun así fallaban, como la división entre cero de la Parte 4, que terminó con `SIGFPE`, o el promedio de la Parte 2, que devolvía 270 en lugar de 90. Otros ni siquiera fallaban visiblemente, como la fuga de memoria o el acceso fuera de límites, y solo aparecieron con Valgrind o ASan.

**3. ¿Cuál fue la diferencia entre un error de sintaxis y un error lógico?**

El de sintaxis rompe las reglas del lenguaje y el compilador lo detecta antes de ejecutar, indicando archivo, línea y causa probable. Fue el caso del `;` faltante, que impidió generar el ejecutable. El error lógico es código válido que hace algo distinto a lo que se quería. No produce ningún aviso, y solo se detecta al comparar el resultado con lo esperado, como el promedio que no dividía entre la cantidad de datos.

**4. ¿Qué ventaja tiene usar `gdb`?**

Permite ver el programa por dentro mientras corre. Se puede detener en una línea, avanzar paso a paso, mirar el valor de las variables y revisar la pila de llamadas con `backtrace`. Es la herramienta adecuada para entender por qué un valor es incorrecto, como cuando se vio que `mayor` nunca cambiaba de 0 en el reto, o para ubicar la línea de una falla como la división entre cero.

**5. ¿Qué ventaja tiene usar `valgrind`?**

Revisa los accesos a memoria y las reservas sin necesidad de recompilar el programa, y entrega un informe con la línea del error y la de la reserva del bloque. Con una sola ejecución encontró en el reto el acceso inválido y la fuga de memoria. Su desventaja es que hace el programa bastante más lento.

**6. ¿Qué ventaja tiene usar AddressSanitizer?**

Detiene el programa justo en el primer acceso inválido y lo describe con precisión, con nombres como `heap-buffer-overflow` o `heap-use-after-free`, e incluye dónde se reservó y dónde se liberó el bloque. Es más rápido que Valgrind, pero exige recompilar con `-fsanitize=address`. Además, como aborta en el primer error, puede ocultar otros que vengan después, como pasó con la fuga en el reto.

**7. ¿Qué aprendió sobre errores de memoria?**

Que muchos no se notan al ejecutar. Una fuga no cambia la salida, un acceso fuera de límites puede leer un valor que casualmente es inofensivo, y un uso después de liberar puede incluso imprimir el valor correcto porque la memoria aún no fue reutilizada. Son comportamiento indefinido, por lo que el programa puede funcionar en una ejecución y fallar en otra. También aprendió que cada `new` debe tener su `delete` (y `new[]` su `delete[]`), y que asignar `nullptr` tras liberar evita punteros colgantes.

**8. ¿Qué aprendió sobre errores de concurrencia?**

Que el resultado depende de cómo se intercalan los hilos y por eso cambia entre ejecuciones. En la Parte 7 el contador dio 7 valores distintos en 8 corridas y ninguno fue 200 000. También que `contador++` no es atómico, que un resultado correcto no prueba que no haya carrera, ya que ThreadSanitizer y Helgrind la reportaron aun cuando el contador dio 200 000, y que un `std::mutex` con `std::lock_guard` resuelve el problema a costa de algo de rendimiento.

**9. ¿Cuál fue el error más fácil de encontrar?**

El error de sintaxis de la Parte 2. El compilador indicó el archivo, la línea y lo que faltaba, y la corrección fue agregar un `;`. No hizo falta ninguna herramienta adicional.

**10. ¿Cuál fue el error más difícil de entender?**

La condición de carrera de la Parte 7. El código se ve correcto, no hay ningún error evidente y el resultado cambia en cada ejecución. Fue necesario entender que `contador++` se compone de varios pasos y que los hilos pueden intercalarse entre ellos. Además, TSan y Helgrind reportaron la carrera incluso en ejecuciones donde el resultado salió bien, lo que va contra la intuición de que "si el resultado es correcto, no hay error".

**11. ¿Qué herramienta usaría primero si un programa produce un resultado incorrecto?**

`gdb`. Si el programa no falla pero da un valor equivocado, el problema es de lógica y las herramientas de memoria no lo van a detectar. Con `gdb` se pone un breakpoint antes del cálculo y se revisan los valores de las variables paso a paso, como se hizo con `mayor` y `suma` en el reto. Antes de eso conviene compilar con `-Wall -Wextra`, porque a veces el compilador ya avisa.

**12. ¿Qué herramienta usaría primero si un programa falla con segmentation fault?**

AddressSanitizer, ya que apunta directamente a la línea del acceso inválido y al tipo de error, y es más rápido que Valgrind. Si no se puede recompilar, Valgrind cumple la misma función sobre el binario existente. `gdb` también sirve, porque se detiene en la línea de la falla y con `backtrace` muestra cómo se llegó hasta ahí.

**13. ¿Qué herramienta usaría primero si sospecha una pérdida de memoria?**

Valgrind con `--leak-check=full`, porque informa cuántos bytes se perdieron y en qué línea se reservó cada bloque, como los 20 bytes del reto. ASan también puede detectar fugas con su componente LeakSanitizer, pero en este laboratorio la herramienta que las mostró con claridad fue Valgrind.

**14. ¿Qué herramienta usaría primero si sospecha un problema con hilos?**

ThreadSanitizer, porque en este caso entregó el reporte más claro, con los dos hilos involucrados, la variable y la línea. Helgrind es una buena alternativa cuando no se quiere recompilar. En ambos casos hay que ejecutar el programa varias veces o con distintas cargas, ya que un error de concurrencia puede no manifestarse siempre.

**15. ¿Cómo aplicaría estas herramientas en proyectos futuros?**

Compilando siempre con `-Wall -Wextra` para atrapar errores de sintaxis y advertencias desde el inicio, y usando ASan durante el desarrollo y las pruebas, ya que su costo es bajo. Valgrind se usaría para revisar fugas antes de entregar, y ThreadSanitizer en cualquier código con hilos. `gdb` quedaría para entender fallos y resultados incorrectos. Sobre todo, aplicaría el orden que recomienda el laboratorio, que es observar y reproducir el problema antes de tocar el código, y verificar después de cada corrección.

---

## Referencias

[1] Valgrind Developers, "Memcheck: a memory error detector", en Valgrind User Manual. https://valgrind.org/docs/manual/mc-manual.html

[2] Clang Project, "AddressSanitizer", en Clang Documentation. https://clang.llvm.org/docs/AddressSanitizer.html

[3] GDB, "Continuing and Stepping", en Debugging with GDB. https://sourceware.org/gdb/current/onlinedocs/gdb.html/Continuing-and-Stepping.html

[4] GDB, "Setting Breakpoints", en Debugging with GDB. https://sourceware.org/gdb/current/onlinedocs/gdb.html/Set-Breaks.html

[5] cppreference, "Undefined behavior", en C++ Language. https://en.cppreference.com/w/cpp/language/ub

[6] cppreference, "Arithmetic operators". https://en.cppreference.com/w/cpp/language/operator_arithmetic

[7] cppreference, "delete expression". https://en.cppreference.com/w/cpp/language/delete
