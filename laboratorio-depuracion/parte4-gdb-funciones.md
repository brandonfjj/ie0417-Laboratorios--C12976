# Parte 4: step, next y backtrace

## Objetivo

Comprender la diferencia entre avanzar línea por línea sin entrar en funciones y entrar dentro de funciones. Además, usar `backtrace` para revisar la pila de llamadas.

---

## Código base

### Código original

Archivo [`codigo/04_gdb_funciones.cpp`](codigo/04_gdb_funciones.cpp). Aquí se muestra la versión original, con el error.

```cpp
#include <iostream>

int dividir(int a, int b) {
    int resultado = a / b;
    return resultado;
}

int calcular(int x) {
    int divisor = x - 5;
    int resultado = dividir(100, divisor);
    return resultado;
}

int main() {
    int valor = 5;
    int respuesta = calcular(valor);

    std::cout << "Respuesta: " << respuesta << std::endl;

    return 0;
}
```

### Comando de compilación

```bash
g++ -g -o gdb_funciones codigo/04_gdb_funciones.cpp
```

### Comando de ejecución normal

```bash
./gdb_funciones
```

### Error observado al ejecutar normalmente

El programa compila sin problemas, pero al ejecutarlo se termina de forma abrupta.

```text
Floating point exception (core dumped)
```

El código de salida fue 136, que equivale a 128 más el número de la señal `SIGFPE` (8). Es un error de ejecución, porque el compilador no detectó nada. Además, el mensaje no dice en qué línea ni por qué ocurrió, solo que hubo una excepción aritmética. Para saberlo hace falta un depurador.

---

## Uso de gdb

### Comandos usados en gdb

#### Diferencia entre `next` y `step`

Ambos avanzan una línea del código fuente, pero se comportan distinto cuando la línea contiene una llamada a una función. `next` ejecuta la función completa y se detiene en la siguiente línea de la función actual. `step` entra a la función y se detiene en su primera línea, siempre que la función tenga información de depuración [1].

En la sesión se vio con claridad. En la línea 16 de `main` (`int respuesta = calcular(valor);`) se usó `step` y `gdb` entró a `calcular (x=5)`. Después, en la línea 10 de `calcular`, otro `step` entró a `dividir (a=100, b=0)`. Si en lugar de `step` se hubiera usado `next`, la división por cero habría ocurrido dentro de la función sin que `gdb` mostrara dónde.

#### Qué hace `backtrace`

Muestra un resumen de cómo llegó el programa al punto donde está detenido [2]. Imprime una línea por cada función activa en ese momento, empezando por la que se está ejecutando (marco 0) y siguiendo con la función que la llamó (marco 1), y así hasta `main` [2]. Cada línea indica el número de marco, la función, sus argumentos, el archivo y la línea. Esa lista de llamadas es la pila de llamadas, y cada una de sus entradas es un marco que guarda los argumentos y las variables locales de esa función [3].

### Salida de terminal del uso de gdb

```text
$ gdb ./gdb_funciones
GNU gdb (Ubuntu 12.1-0ubuntu1~22.04.2) 12.1
...
Reading symbols from ./gdb_funciones...
(gdb) break main
Breakpoint 1 at 0x1221: file codigo/04_gdb_funciones.cpp, line 15.
(gdb) run
Starting program: /home/brandonfj/repos/ie0417-laboratorios-C12976/laboratorio-depuracion/gdb_funciones
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".

Breakpoint 1, main () at codigo/04_gdb_funciones.cpp:15
15          int valor = 5;
(gdb) next
16          int respuesta = calcular(valor);
(gdb) step
calcular (x=5) at codigo/04_gdb_funciones.cpp:9
9           int divisor = x - 5;
(gdb) step
10          int resultado = dividir(100, divisor);
(gdb) print x
$1 = 5
(gdb) print divisor
$2 = 0
(gdb) step
dividir (a=100, b=0) at codigo/04_gdb_funciones.cpp:4
4           int resultado = a / b;
(gdb) print a
$3 = 100
(gdb) print b
$4 = 0
(gdb) backtrace
#0  dividir (a=100, b=0) at codigo/04_gdb_funciones.cpp:4
#1  0x000055555555520d in calcular (x=5) at codigo/04_gdb_funciones.cpp:10
#2  0x0000555555555232 in main () at codigo/04_gdb_funciones.cpp:16
(gdb) quit
A debugging session is active.

        Inferior 1 [process 34476] will be killed.

Quit anyway? (y or n) y
```

### Valores observados

| Variable  | Función    | Valor observado | Momento en que se observó                     |
|-----------|------------|-----------------|-----------------------------------------------|
| `valor`   | `main`     | 5               | Se asigna en la línea 15 y se pasa a `calcular` |
| `x`       | `calcular` | 5               | Detenido en la línea 10                       |
| `divisor` | `calcular` | 0               | Detenido en la línea 10                       |
| `a`       | `dividir`  | 100             | Detenido en la línea 4                        |
| `b`       | `dividir`  | 0               | Detenido en la línea 4                        |

El valor de `valor` se deduce de que `calcular` recibió `x=5`, ya que `main` la llama con `calcular(valor)`.

### Resultado de `backtrace`

```text
#0  dividir (a=100, b=0) at codigo/04_gdb_funciones.cpp:4
#1  0x000055555555520d in calcular (x=5) at codigo/04_gdb_funciones.cpp:10
#2  0x0000555555555232 in main () at codigo/04_gdb_funciones.cpp:16
```

| Marco | Función    | Línea | Significado                                             |
|-------|------------|-------|---------------------------------------------------------|
| #0    | `dividir`  | 4     | Función donde está detenido el programa, con `b=0`      |
| #1    | `calcular` | 10    | Llamó a `dividir(100, divisor)`                         |
| #2    | `main`     | 16    | Llamó a `calcular(valor)` y es el inicio del programa   |

Los marcos 1 y 2 muestran además una dirección (`0x000055555555520d`), que es la instrucción a la que se vuelve cuando termina la función llamada. En el marco 0 no aparece porque el programa está al inicio de la línea.

### Qué se encontró usando gdb

1. En `main`, `valor` vale 5 y se envía a `calcular`.
2. En `calcular`, `divisor` se calcula como `x - 5`, que es 5 - 5 = 0.
3. `calcular` llama a `dividir(100, divisor)`, así que en `dividir` el parámetro `b` vale 0.
4. La línea 4 de `dividir` ejecuta `a / b`, es decir 100 / 0, una división entre cero.

Si se deja que el programa siga dentro de `gdb`, se detiene justo en esa línea con el mensaje `Program received signal SIGFPE, Arithmetic exception.`, lo que confirma que la división de la línea 4 es la causa.

---

## Corrección

### Explicación de la corrección

Se agregó una validación al inicio de `dividir`. Si `b` es 0, la función muestra un mensaje de error y retorna 0, sin llegar a ejecutar la división. Con eso el programa ya no se cae.

### Código corregido

```cpp
#include <iostream>

int dividir(int a, int b) {
    if (b == 0) {
        std::cout << "Error: no se puede dividir entre cero." << std::endl;
        return 0;
    }

    int resultado = a / b;
    return resultado;
}

int calcular(int x) {
    int divisor = x - 5;
    int resultado = dividir(100, divisor);
    return resultado;
}

int main() {
    int valor = 5;
    int respuesta = calcular(valor);

    std::cout << "Respuesta: " << respuesta << std::endl;

    return 0;
}
```

### Resultado final

```bash
$ g++ -std=c++17 -Wall -Wextra -g -o gdb_funciones codigo/04_gdb_funciones.cpp
$ ./gdb_funciones
Error: no se puede dividir entre cero.
Respuesta: 0
```

El programa terminó normalmente, con código de salida 0, y mostró el mensaje de error en lugar de caerse. Queda una limitación. El resultado "Respuesta: 0" es engañoso, porque parece una respuesta válida cuando en realidad la operación no se pudo realizar.

---

## Preguntas de reflexión

**1. ¿Qué diferencia observó entre `next` y `step`?**

`next` ejecuta de una vez toda la función que se llama en la línea y se detiene en la línea siguiente, mientras que `step` entra a la función y se detiene en su primera línea. En el ejercicio, `step` permitió bajar de `main` a `calcular` y luego a `dividir`, y así ver que `b` valía 0. Con `next` esos valores no se habrían visto.

**2. ¿Para qué sirve `backtrace`?**

Sirve para ver la cadena de funciones que llevó al programa hasta el punto donde está detenido [2]. En este caso mostró que `dividir` fue llamada por `calcular` en la línea 10, y esta por `main` en la línea 16. Es muy útil cuando un programa falla dentro de una función que se usa en varios lugares, porque ayuda a saber quién la llamó y con qué argumentos.

**3. ¿Cuál fue la causa del error?**

Una división entre cero. En `calcular`, `divisor` se calcula como `x - 5` y como `x` valía 5, `divisor` quedó en 0. Ese valor se envió a `dividir` como `b`, y la operación `a / b` de la línea 4 dividió 100 entre 0.

**4. ¿Por qué este error es de tiempo de ejecución?**

Porque el código es válido y el compilador no tenía forma de detectarlo. El problema depende de los valores que tienen las variables mientras el programa corre, y solo aparece al ejecutar la línea de la división con `b` igual a 0. En C++, dividir un entero entre cero es comportamiento indefinido [4], y en este caso el sistema terminó el programa con la señal `SIGFPE`.

**5. ¿Cómo podría prevenirse este tipo de error desde el diseño del programa?**

Validando los datos antes de usarlos, como se hizo con la condición `b == 0`. Además, conviene que la función indique de alguna forma que falló, por ejemplo con una excepción o devolviendo un valor opcional, en lugar de retornar 0, que se puede confundir con un resultado válido. También ayuda definir qué valores acepta cada función, y probar casos límite como el divisor 0 desde el inicio.

---

## Referencias

[1] GDB, "Continuing and Stepping", en Debugging with GDB. https://sourceware.org/gdb/current/onlinedocs/gdb.html/Continuing-and-Stepping.html

[2] GDB, "Backtraces", en Debugging with GDB. https://sourceware.org/gdb/current/onlinedocs/gdb.html/Backtrace.html

[3] GDB, "Examining the Stack", en Debugging with GDB. https://sourceware.org/gdb/current/onlinedocs/gdb.html/Stack.html

[4] cppreference, "Arithmetic operators". https://en.cppreference.com/w/cpp/language/operator_arithmetic
