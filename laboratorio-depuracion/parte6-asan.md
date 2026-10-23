# Parte 6: AddressSanitizer

## Objetivo

Usar AddressSanitizer para detectar errores de memoria al ejecutar un programa.

---

## Código base

### Código original


```cpp
#include <iostream>

int main() {
    int* numero = new int;
    *numero = 42;

    delete numero;

    std::cout << "Número: " << *numero << std::endl;

    return 0;
}
```

### Qué hace el programa

Reserva un entero en el heap con `new int` y le asigna el valor 42. Después libera esa memoria con `delete` y, a pesar de eso, intenta leer el valor a través del mismo puntero para imprimirlo. El defecto está en esa lectura, porque el bloque ya fue devuelto al sistema.

### Comando de compilación normal

```bash
g++ -g -o use_after_free codigo/07_use_after_free.cpp
```

### Comando de ejecución normal

```bash
./use_after_free
```

### Qué ocurrió al ejecutar sin sanitizer

```text
Número: -326450338
```

El programa compila sin advertencias y termina con código de salida 0. Sin embargo, el número impreso no es 42 sino un valor sin sentido. Al liberar el bloque, el asignador de memoria reutiliza esos bytes para sus propios datos de control, y la lectura posterior recoge ese contenido. El valor impreso es basura y puede cambiar de una ejecución a otra, además de depender del sistema. Aquí al menos hubo una pista visible, pero en otros programas el valor viejo puede seguir intacto en memoria y el error pasaría completamente inadvertido.

### Herramienta usada

AddressSanitizer, un detector rápido de errores de memoria que se activa al compilar con `-fsanitize=address` [1][2]. Combina instrumentación del compilador, que revisa cada acceso a memoria, con una biblioteca en tiempo de ejecución que reemplaza a `malloc` y `free` para llevar el control de los bloques [1]. Se compila además con `-g` para obtener archivo y línea en el reporte [2].

```bash
g++ -g -fsanitize=address -o use_after_free_asan codigo/07_use_after_free.cpp
./use_after_free_asan
```

### Qué ocurrió al ejecutar con AddressSanitizer

El programa se detuvo antes de imprimir el número, con código de salida 1, y mostró el siguiente reporte. Se omite la leyenda de bytes sombra del final por ser texto fijo de la herramienta.

```text
=================================================================
==11036==ERROR: AddressSanitizer: heap-use-after-free on address 0x502000000010 at pc 0x6205278673ce bp 0x7ffcc299db00 sp 0x7ffcc299daf0
READ of size 4 at 0x502000000010 thread T0
    #0 0x6205278673cd in main codigo/07_use_after_free.cpp:9
    #1 0x7979d4029d8f in __libc_start_call_main ../sysdeps/nptl/libc_start_call_main.h:58
    #2 0x7979d4029e3f in __libc_start_main_impl ../csu/libc-start.c:392
    #3 0x620527867244 in _start (/home/brandonfj/repos/ie0417-laboratorios-C12976/laboratorio-depuracion/use_after_free_asan+0x1244)

0x502000000010 is located 0 bytes inside of 4-byte region [0x502000000010,0x502000000014)
freed by thread T0 here:
    #0 0x7979d48b724f in operator delete(void*, unsigned long) ../../../../src/libsanitizer/asan/asan_new_delete.cpp:172
    #1 0x620527867379 in main codigo/07_use_after_free.cpp:7
    #2 0x7979d4029d8f in __libc_start_call_main ../sysdeps/nptl/libc_start_call_main.h:58

previously allocated by thread T0 here:
    #0 0x7979d48b61e7 in operator new(unsigned long) ../../../../src/libsanitizer/asan/asan_new_delete.cpp:99
    #1 0x62052786731e in main codigo/07_use_after_free.cpp:4
    #2 0x7979d4029d8f in __libc_start_call_main ../sysdeps/nptl/libc_start_call_main.h:58

SUMMARY: AddressSanitizer: heap-use-after-free codigo/07_use_after_free.cpp:9 in main
Shadow bytes around the buggy address:
  0x0a047fff7fb0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  ...
=>0x0a047fff8000: fa fa[fd]fa fa fa fa fa fa fa fa fa fa fa fa fa
  ...
==11036==ABORTING
```

### Análisis del reporte

| Dato del reporte | Qué indica |
|---|---|
| `heap-use-after-free` | Tipo de error. Se usó un bloque del heap que ya había sido liberado. |
| `READ of size 4 ... main codigo/07_use_after_free.cpp:9` | La lectura de 4 bytes (un `int`) ocurrió en la línea 9, en `*numero` dentro del `std::cout`. |
| `0 bytes inside of 4-byte region` | La dirección leída es el inicio del bloque de 4 bytes que se había reservado. |
| `freed by thread T0 here ... 07_use_after_free.cpp:7` | El bloque se liberó en la línea 7, con `delete numero;`. |
| `previously allocated by thread T0 here ... 07_use_after_free.cpp:4` | El bloque se reservó en la línea 4, con `new int`. |
| `[fd]` en los bytes sombra | Marca la zona como "Freed heap region", es decir, memoria liberada. Los bytes `fa` que la rodean son zonas de protección (redzones) del heap. |

Lo más útil es que el reporte da la historia completa del bloque en tres puntos. Dónde se reservó, dónde se liberó y dónde se usó indebidamente. Con eso se identifica de inmediato que la línea 9 está leyendo algo que la línea 7 ya devolvió.

### Qué significa `heap-use-after-free`

Significa que el programa accedió, ya sea leyendo o escribiendo, a memoria dinámica que ya había sido liberada [1]. El puntero `numero` sigue guardando la misma dirección después del `delete`, pero esa dirección ya no pertenece al programa. A este puntero se le llama puntero colgante. Usarlo es comportamiento indefinido en C++ [5].

### Explicación del error

El orden de las operaciones estaba mal. El `delete numero;` se ejecutó antes de la última vez que se necesitaba el valor. Como `delete` no modifica el puntero, el código siguió compilando y la lectura parecía válida, pero apuntaba a memoria ya devuelta. ASan lo detecta porque, al liberar un bloque, marca esa zona como inaccesible, y cada acceso instrumentado consulta esa marca antes de ejecutarse [1].

### Corrección

Se movió el `std::cout` para que el valor se imprima antes de liberar la memoria. Después del `delete` se asignó `nullptr` al puntero, para que no quede colgante y un uso accidental posterior falle de forma evidente en lugar de leer basura.

### Código corregido

```cpp
#include <iostream>

int main() {
    int* numero = new int;
    *numero = 42;

    std::cout << "Número: " << *numero << std::endl;

    delete numero;
    numero = nullptr;

    return 0;
}
```

### Evidencia después de corregir

```bash
$ g++ -g -o use_after_free codigo/07_use_after_free.cpp
$ ./use_after_free
Número: 42
$ g++ -g -fsanitize=address -o use_after_free_asan codigo/07_use_after_free.cpp
$ ./use_after_free_asan
Número: 42
$ echo $?
0
```

Ahora el programa imprime el valor esperado, tanto compilado normalmente como con ASan, y este último no emite ningún reporte. También se revisó con `valgrind --leak-check=full ./use_after_free`, que terminó con `All heap blocks were freed -- no leaks are possible` y `ERROR SUMMARY: 0 errors from 0 contexts`.

### Comparación con valgrind

Para responder la pregunta 4 se ejecutó también la versión original con `valgrind`.

```bash
g++ -g -o use_after_free codigo/07_use_after_free.cpp
valgrind --leak-check=full ./use_after_free
```

```text
==11037== Invalid read of size 4
==11037==    at 0x109263: main (07_use_after_free.cpp:9)
==11037==  Address 0x4dd4c80 is 0 bytes inside a block of size 4 free'd
==11037==    at 0x484BB6F: operator delete(void*, unsigned long) (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==11037==    by 0x109242: main (07_use_after_free.cpp:7)
==11037==  Block was alloc'd at
==11037==    at 0x4849013: operator new(unsigned long) (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==11037==    by 0x10921E: main (07_use_after_free.cpp:4)
==11037==
Número: 42
...
==11037== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
```

### Reflexión del ejercicio

Lo que más llamó la atención fue que la ejecución normal imprimió un número raro pero no se detuvo, mientras que ASan frenó el programa justo en la lectura inválida. También sorprendió que `valgrind` imprimiera 42 en la misma situación, lo que muestra que el valor que se lee de memoria liberada no es confiable y depende del entorno. Por eso conviene apoyarse en una herramienta y no en la salida del programa.

---

## Preguntas de reflexión

**1. ¿Qué significa usar memoria después de liberarla?**

Significa leer o escribir a través de un puntero cuyo bloque ya fue devuelto con `delete` o `free`. El puntero conserva la dirección, pero el bloque ya no es del programa y el asignador puede dárselo a otra reserva. En el ejercicio, `*numero` se leyó en la línea 9 después de haber liberado el bloque en la línea 7. Es comportamiento indefinido [5].

**2. ¿Por qué este error puede ser difícil de detectar sin herramientas?**

Porque el efecto depende de qué haya pasado con esa memoria después de liberarla. Puede seguir conteniendo el valor viejo y entonces el programa parece funcionar, como ocurrió con `valgrind`, que imprimió 42. También puede contener basura, como en la ejecución normal, o pertenecer ya a otro objeto y dañarlo sin avisar. Como el resultado cambia entre ejecuciones y entornos, el error puede pasar las pruebas y aparecer solo en producción.

**3. ¿Qué ventaja tiene AddressSanitizer sobre ejecutar el programa normalmente?**

Detiene el programa en el momento exacto del acceso inválido y explica qué pasó, en lugar de dejar que continúe con datos corruptos [1]. El reporte incluye la línea del acceso, la de la liberación y la de la reserva. Además, no depende de que el error produzca un síntoma visible, porque cada acceso a memoria se revisa. Su costo es un programa más lento, con una desaceleración típica de unas 2 veces según la documentación [1].

**4. ¿Qué diferencia observó entre el reporte de `valgrind` y el reporte de AddressSanitizer?**

Ambos entregaron la misma información de fondo, que es la línea del acceso (9), la de la liberación (7) y la de la reserva (4). Las diferencias fueron las siguientes.

| Aspecto | valgrind | AddressSanitizer |
|---|---|---|
| Compilación | Se usa el binario normal, solo con `-g` | Hay que recompilar con `-fsanitize=address` |
| Nombre del error | `Invalid read of size 4` | `heap-use-after-free`, que dice el tipo con más precisión |
| Qué hizo el programa | Continuó e imprimió `Número: 42` | Se detuvo con `ABORTING` y código 1 |
| Formato | Reporte corto | Reporte más largo, con direcciones y mapa de bytes sombra |

**5. ¿Por qué es buena práctica asignar `nullptr` después de liberar un puntero?**

Porque deja de existir un puntero colgante con una dirección que ya no es válida. Si luego se usa por error, el acceso a `nullptr` provoca una falla clara e inmediata, que es mucho más fácil de encontrar que leer basura sin darse cuenta. También evita liberar dos veces el mismo bloque por accidente, ya que aplicar `delete` a un puntero nulo no tiene ningún efecto [3]. Esta práctica no reemplaza a ASan, porque otros punteros que apunten al mismo bloque seguirían colgando, pero reduce la probabilidad del error.

---

## Referencias

[1] Clang Project, "AddressSanitizer", en Clang Documentation. https://clang.llvm.org/docs/AddressSanitizer.html

[2] GCC Developers, "Program Instrumentation Options", en Using the GNU Compiler Collection (GCC). https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html

[3] cppreference, "delete expression". https://en.cppreference.com/w/cpp/language/delete

[4] Valgrind Developers, "The Valgrind Quick Start Guide", en Valgrind User Manual. https://valgrind.org/docs/manual/quick-start.html

[5] cppreference, "Undefined behavior", en C++ Language. https://en.cppreference.com/w/cpp/language/ub
