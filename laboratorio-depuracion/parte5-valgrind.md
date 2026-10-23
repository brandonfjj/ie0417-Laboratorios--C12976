# Parte 5: análisis de memoria con valgrind

## Objetivo

Usar `valgrind` para detectar pérdidas de memoria y errores de acceso a memoria. Se cubren dos ejercicios, la pérdida de memoria y el acceso fuera de límites.

---

## Ejercicio 1: pérdida de memoria

### Código original

Archivo [`codigo/05_memory_leak.cpp`](codigo/05_memory_leak.cpp). Aquí se muestra la versión original, con el error. El `.cpp` del repositorio quedó con la versión corregida.

```cpp
#include <iostream>

void crear_arreglo() {
    int* datos = new int[10];

    for (int i = 0; i < 10; i++) {
        datos[i] = i * 2;
    }

    std::cout << "Primer elemento: " << datos[0] << std::endl;
}

int main() {
    crear_arreglo();
    return 0;
}
```

### Qué hace el programa

La función `crear_arreglo` pide en el heap un arreglo de 10 enteros con `new int[10]`, lo llena con los múltiplos de 2 (0, 2, 4, ..., 18) y muestra el primer elemento. Luego `main` la llama una sola vez y termina.

### Comando de compilación

```bash
g++ -g -o memory_leak codigo/05_memory_leak.cpp
```

### Comando de ejecución

```bash
./memory_leak
```

### Error observado al ejecutar normalmente

```text
Primer elemento: 0
```

El programa compila sin advertencias, se ejecuta y termina con código de salida 0. Nada en la salida delata el problema, ya que la pérdida de memoria no produce ninguna advertencia visible.

### Herramienta usada

`valgrind` con la función Memcheck, que revisa el uso de memoria dinámica y, al terminar el programa, informa qué bloques del heap quedaron sin liberar [1]. Se compiló con `-g` para que los reportes muestren archivo y línea.

```bash
valgrind --leak-check=yes ./memory_leak
```

### Qué reportó valgrind

```text
==3479== Memcheck, a memory error detector
==3479== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==3479== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==3479== Command: ./memory_leak
==3479==
Primer elemento: 0
==3479==
==3479== HEAP SUMMARY:
==3479==     in use at exit: 40 bytes in 1 blocks
==3479==   total heap usage: 3 allocs, 2 frees, 76,840 bytes allocated
==3479==
==3479== 40 bytes in 1 blocks are definitely lost in loss record 1 of 1
==3479==    at 0x484A2F3: operator new[](unsigned long) (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==3479==    by 0x1091FE: crear_arreglo() (05_memory_leak.cpp:4)
==3479==    by 0x10927E: main (05_memory_leak.cpp:15)
==3479==
==3479== LEAK SUMMARY:
==3479==    definitely lost: 40 bytes in 1 blocks
==3479==    indirectly lost: 0 bytes in 0 blocks
==3479==      possibly lost: 0 bytes in 0 blocks
==3479==    still reachable: 0 bytes in 0 blocks
==3479==         suppressed: 0 bytes in 0 blocks
==3479==
==3479== For lists of detected and suppressed errors, rerun with: -s
==3479== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
```

```bash
valgrind --leak-check=full --show-leak-kinds=all ./memory_leak
```

En este programa la salida fue idéntica a la anterior, porque la única fuga es de tipo `definitely lost` y no hay bloques de las otras categorías que `--show-leak-kinds=all` pudiera mostrar. La opción `--leak-check=full` es la que pide el detalle de cada bloque perdido, incluyendo dónde se reservó [1].

### Análisis del reporte

| Dato del reporte | Qué indica |
|---|---|
| `in use at exit: 40 bytes in 1 blocks` | Al terminar quedó 1 bloque sin liberar. Son 40 bytes. |
| `3 allocs, 2 frees` | Hubo una reserva más que liberaciones. |
| `operator new[]` | El bloque se reservó con `new[]`. |
| `crear_arreglo() (05_memory_leak.cpp:4)` | La reserva ocurrió en la línea 4, justo donde se escribe `new int[10]`. |
| `main (05_memory_leak.cpp:15)` | `crear_arreglo` fue llamada desde la línea 15 de `main`. |
| `definitely lost: 40 bytes in 1 blocks` | Esos 40 bytes no se pueden recuperar. |

### Qué significa `definitely lost`

Significa que el programa terminó sin liberar el bloque y que ya no existe ningún puntero que apunte a él [1]. En este caso, `datos` es una variable local de `crear_arreglo`. Cuando la función retorna, esa variable desaparece y con ella la única dirección del bloque, de modo que el programa ya no podría liberarlo aunque quisiera. Por eso `valgrind` lo clasifica como perdido con certeza, a diferencia de `still reachable`, donde sí queda algún puntero al bloque, o `possibly lost`, donde solo queda un puntero al interior del bloque [1].

### Explicación del error

El arreglo se reservó con `new[]` pero nunca se liberó con `delete[]`. Cada `new` debe tener su `delete` correspondiente, y la forma de arreglo se libera con `delete[]` [2]. Como la función termina sin liberar, esos 40 bytes quedan ocupados hasta que el proceso muere.

### Corrección

Se agregó `delete[] datos;` al final de `crear_arreglo`, después de la última vez que se usa `datos` (el `std::cout`) y antes de que la función retorne. Se usa la forma `delete[]` porque la memoria se obtuvo con `new[]`. Mezclar las formas es comportamiento indefinido [2].

### Código corregido

```cpp
#include <iostream>

void crear_arreglo() {
    int* datos = new int[10];

    for (int i = 0; i < 10; i++) {
        datos[i] = i * 2;
    }

    std::cout << "Primer elemento: " << datos[0] << std::endl;
    delete[] datos;
}

int main() {
    crear_arreglo();
    return 0;
}
```

### Evidencia después de corregir

```bash
$ g++ -g -o memory_leak codigo/05_memory_leak.cpp
$ ./memory_leak
Primer elemento: 0
$ valgrind --leak-check=full --show-leak-kinds=all ./memory_leak
```

```text
==3420== Memcheck, a memory error detector
==3420== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==3420== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==3420== Command: ./memory_leak
==3420==
Primer elemento: 0
==3420==
==3420== HEAP SUMMARY:
==3420==     in use at exit: 0 bytes in 0 blocks
==3420==   total heap usage: 3 allocs, 3 frees, 76,840 bytes allocated
==3420==
==3420== All heap blocks were freed -- no leaks are possible
==3420==
==3420== For lists of detected and suppressed errors, rerun with: -s
==3420== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

Ahora las reservas y las liberaciones coinciden (3 allocs, 3 frees), no queda nada en uso al salir y `valgrind` informa 0 errores. La salida del programa no cambió, lo que confirma que el error no se notaba al ejecutar y solo se veía con la herramienta.

### Reflexión del ejercicio

Lo más importante fue ver que el programa funcionaba y aun así tenía un defecto real. Sin `valgrind` la pérdida habría pasado inadvertida. También quedó claro que el reporte con `-g` apunta directo a la línea de la reserva, lo que facilita saber qué `delete[]` falta y dónde colocarlo.

---

## Preguntas de reflexión del ejercicio 1

**1. ¿Qué es una pérdida de memoria?**

Ocurre cuando el programa reserva memoria dinámica y posteriormente deja de tener forma de acceder a ella sin haberla liberado [1]. El bloque sigue marcado como ocupado, pero nadie lo puede usar ni puede devolserve. En el ejercicio, los 40 bytes de `datos` quedaron en esa condición al retornar `crear_arreglo`.

**2. ¿Por qué el programa puede terminar aparentemente bien aunque tenga una pérdida de memoria?**

Porque la fuga no altera la lógica del programa ni provoca una falla inmediata. El arreglo se usó correctamente, se imprimió el valor esperado y el código de salida fue 0. Además, cuando el proceso termina el sistema operativo recupera toda su memoria, así que el efecto de una fuga pequeña desaparece al salir. El problema solo es visible con una herramienta que revise el heap, como `valgrind`.

**3. ¿Qué significa liberar memoria dinámica?**

Significa devolver al sistema un bloque que se había reservado con `new`, para que pueda reutilizarse. Se hace con `delete` si se reservó un solo objeto, o con `delete[]` si se reservó un arreglo [2].

**4. ¿Por qué se usa `delete[]` y no solo `delete`?**

Porque la forma debe corresponder a la que se usó al reservar. `delete[]` destruye un arreglo creado con `new[]` y, si los elementos tienen destructor, lo invoca para cada uno [2]. Usar `delete` sobre un arreglo, o `delete[]` sobre un objeto individual, es comportamiento indefinido [2]. Con enteros como en este ejercicio no hay destructores que ejecutar, pero la regla se mantiene igual y conviene seguirla siempre.

**5. ¿Qué tipo de problemas podrían aparecer en programas grandes si no se libera memoria?**

En un programa que corre por mucho tiempo o que repite la reserva muchas veces, la memoria ocupada puede crecer sin parar. Esto hace que el programa use cada vez más RAM, que el sistema empiece a usar el disco como memoria o que, al final, las reservas fallen y el proceso termine. Además, en un programa grande una fuga es más difícil de encontrar, por lo que conviene detectarla pronto con herramientas como `valgrind`.

---

## Ejercicio 2: acceso fuera de límites

### Código original

```cpp
#include <iostream>

int main() {
    int* arreglo = new int[5];

    for (int i = 0; i <= 5; i++) {
        arreglo[i] = i * 10;
    }

    std::cout << "Programa finalizado" << std::endl;

    delete[] arreglo;

    return 0;
}
```

### Qué hace el programa

Reserva en el heap un arreglo de 5 enteros con `new int[5]` y lo llena con los valores 0, 10, 20, 30, 40 y 50 mediante un ciclo. Luego imprime un mensaje, libera el arreglo con `delete[]` y termina. Hasta aquí la memoria se libera bien, por lo que el defecto no es una fuga sino otro distinto.

### Comando de compilación

```bash
g++ -g -o invalid_access codigo/06_invalid_access.cpp
```

### Comando de ejecución

```bash
./invalid_access
```

### Error observado al ejecutar normalmente

```text
Programa finalizado
```

El programa compila sin advertencias, imprime su mensaje y termina con código de salida 0.
### Herramienta usada

`valgrind` con Memcheck, que revisa cada lectura y escritura de memoria que hace el programa y avisa cuando se hace en una dirección que no debería usarse [1].

```bash
valgrind --leak-check=full ./invalid_access
```

### Qué reportó valgrind

```text
==1907== Memcheck, a memory error detector
==1907== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==1907== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==1907== Command: ./invalid_access
==1907==
==1907== Invalid write of size 4
==1907==    at 0x10922D: main (06_invalid_access.cpp:7)
==1907==  Address 0x4dd4c94 is 0 bytes after a block of size 20 alloc'd
==1907==    at 0x484A2F3: operator new[](unsigned long) (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==1907==    by 0x1091FE: main (06_invalid_access.cpp:4)
==1907==
Programa finalizado
==1907==
==1907== HEAP SUMMARY:
==1907==     in use at exit: 0 bytes in 0 blocks
==1907==   total heap usage: 3 allocs, 3 frees, 76,820 bytes allocated
==1907==
==1907== All heap blocks were freed -- no leaks are possible
==1907==
==1907== For lists of detected and suppressed errors, rerun with: -s
==1907== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
```

### Análisis del reporte

| Dato del reporte | Qué indica |
|---|---|
| `Invalid write of size 4` | Se hizo una escritura de 4 bytes (un `int`) en una dirección no válida. |
| `main (06_invalid_access.cpp:7)` | La escritura ocurrió en la línea 7, que es `arreglo[i] = i * 10;`. |
| `0 bytes after a block of size 20 alloc'd` | La dirección escrita está justo al terminar un bloque de 20 bytes, que son los 5 enteros reservados. |
| `operator new[]` en `main (06_invalid_access.cpp:4)` | Ese bloque es el que se reservó en la línea 4 con `new int[5]`. |
| `All heap blocks were freed` | No hay fuga, el `delete[]` sí se ejecutó. |
| `1 errors from 1 contexts` | Se reportó una sola vez porque solo una iteración, la de `i == 5`, escribe fuera. |

El detalle `0 bytes after` es muy útil. Indica que el acceso quedó pegado al final del bloque, que es el patrón típico de un error de uno de más (off by one) al recorrer un arreglo [1]. Los índices válidos de un arreglo de 5 elementos son del 0 al 4, y el ciclo llegó hasta el 5.

### Qué significa `Invalid write`

Significa que el programa escribió en una posición de memoria que no le corresponde, es decir, una dirección que `Memcheck` considera que no debería tocarse [1].

### Explicación del error

La condición `i <= 5` hace que el ciclo ejecute seis iteraciones (i desde 0 hasta 5), pero el arreglo solo tiene cinco posiciones. En la última iteración se escribe `arreglo[5]`, que está un elemento más allá del final del bloque. Acceder fuera de los límites de un arreglo es comportamiento indefinido en C++, así que el estándar no obliga a que el programa falle ni a que dé algún aviso [3].

### Corrección

Se cambió la condición del ciclo de `i <= 5` a `i < 5`, para que el índice recorra solamente del 0 al 4. Se agregó además un comentario en el código que recuerda el rango válido.

### Código corregido

```cpp
#include <iostream>

int main() {
    int* arreglo = new int[5];

    for (int i = 0; i < 5; i++) {
        arreglo[i] = i * 10;
    }

    std::cout << "Programa finalizado" << std::endl;

    delete[] arreglo;

    return 0;
}
```

### Evidencia después de corregir

```bash
$ g++ -g -o invalid_access codigo/06_invalid_access.cpp
$ ./invalid_access
Programa finalizado
$ valgrind --leak-check=full ./invalid_access
```

```text
==1965== Memcheck, a memory error detector
==1965== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==1965== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==1965== Command: ./invalid_access
==1965==
Programa finalizado
==1965==
==1965== HEAP SUMMARY:
==1965==     in use at exit: 0 bytes in 0 blocks
==1965==   total heap usage: 3 allocs, 3 frees, 76,820 bytes allocated
==1965==
==1965== All heap blocks were freed -- no leaks are possible
==1965==
==1965== For lists of detected and suppressed errors, rerun with: -s
==1965== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

El mensaje `Invalid write` desapareció y el resumen indica 0 errores. La salida del programa es exactamente la misma que antes, lo que confirma que el error no era visible desde el programa y solo la herramienta lo exponía.

### Reflexión del ejercicio

Este caso fue más engañoso que la fuga, porque el programa no solo terminó bien sino que además liberó toda su memoria. Sin `valgrind` no había ninguna pista. También quedó claro que el reporte da tres datos que bastan para corregir. La línea exacta del acceso, el tamaño del bloque y la distancia al final del bloque, que apunta directamente a un error de índice.

---

## Preguntas de reflexión del ejercicio 2

**1. ¿Por qué el programa podría no fallar aunque acceda fuera del arreglo?**

Porque C++ no revisa los límites de un arreglo al acceder con `[]`. Así que la escritura en `arreglo[5]` cayó dentro de memoria que el proceso sí tiene mapeada y no hubo falla de segmentación. El sistema solo falla cuando la dirección está en una zona sin permisos, y eso depende de dónde quedó el bloque, algo que no se puede asumir.

**2. ¿Qué significa escribir fuera de los límites de un arreglo?**

Significa guardar un valor en una posición cuyo índice no existe en el arreglo, ya sea negativo o mayor o igual a su tamaño. Esa posición pertenece a otra cosa, o a nada, pero no al arreglo. En el ejercicio, `arreglo[5]` es una posición posterior al último elemento válido, `arreglo[4]`.

**3. ¿Por qué este tipo de error es peligroso?**

Porque puede corromper datos sin avisar. La escritura puede pisar otra variable, o la información que el asignador usa para administrar el heap, y el efecto se manifiesta mucho después y en otra parte del programa, lo que lo vuelve muy difícil de rastrear. Además, es una vulnerabilidad de seguridad conocida, ya que un atacante podría aprovechar una escritura fuera de límites para alterar el comportamiento del programa. Y como es comportamiento indefinido, el resultado puede cambiar con el compilador, las opciones de compilación o hasta con cambios que no tienen relación en el código [3].

**4. ¿Qué diferencia hay entre un error visible y un comportamiento indefinido?**

Un error visible es uno que el programa o el sistema reportan, como un error de compilación o un `Floating point exception` en tiempo de ejecución, y que permite saber que algo falló. Un comportamiento indefinido, en cambio, es una situación en la que el estándar no impone ningún requisito, así que el programa puede fallar, dar resultados incorrectos o aparentar funcionar bien, y la implementación no está obligada a avisar [3]. Este ejercicio es un ejemplo claro. Era comportamiento indefinido, no se vio ningún síntoma y solo `valgrind` lo delató.

---

## Referencias

[1] Valgrind Developers, "Memcheck: a memory error detector", en Valgrind User Manual. https://valgrind.org/docs/manual/mc-manual.html

[2] cppreference, "delete expression". https://en.cppreference.com/w/cpp/language/delete

[3] cppreference, "Undefined behavior", en C++ Language. https://en.cppreference.com/w/cpp/language/ub
