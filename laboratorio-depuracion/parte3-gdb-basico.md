# Parte 3: introducción a gdb

## Objetivo

Usar `gdb` para ejecutar un programa paso a paso, detenerlo en una función e inspeccionar variables.

---

## Código base

### Código original

Archivo [`codigo/03_gdb_basico.cpp`](codigo/03_gdb_basico.cpp).

```cpp
#include <iostream>

int duplicar(int x) {
    int resultado = x * 2;
    return resultado;
}

int sumar(int a, int b) {
    int resultado = a + b;
    return resultado;
}

int main() {
    int numero = 5;
    int doble = duplicar(numero);
    int total = sumar(doble, 10);

    std::cout << "Número: " << numero << std::endl;
    std::cout << "Doble: " << doble << std::endl;
    std::cout << "Total: " << total << std::endl;

    return 0;
}
```

### Comando de compilación

```bash
g++ -g -o gdb_basico codigo/03_gdb_basico.cpp
```

### Comando de ejecución normal

```bash
./gdb_basico
```

### Resultado de la ejecución normal

```text
Número: 5
Doble: 10
Total: 20
```

---

## Uso de gdb

### Para qué sirve `-g`

La opción `-g` le pide al compilador que incluya información de depuración en el ejecutable [1]. Esa información describe el tipo de cada variable y función, y la relación entre los números de línea del código fuente y las direcciones del código máquina [1]. Gracias a eso `gdb` puede mostrar las variables por su nombre y decir en qué línea del `.cpp` está detenido el programa. En la salida de abajo se nota, por ejemplo, en `Breakpoint 1 at 0x120b: file codigo/03_gdb_basico.cpp, line 14.`

### Comandos usados en gdb

#### Qué hace `break main`

Pone un breakpoint en la función `main`. Un breakpoint detiene el programa justo antes de ejecutar la instrucción de esa ubicación, y la ubicación puede ser un nombre de función, un número de línea o una dirección [2]. Con `break main`, `gdb` se detiene al inicio de `main`, que en este programa es la línea 14.

#### Qué hace `run`

Inicia el programa dentro de `gdb` [3]. El programa se ejecuta normalmente hasta que termina o hasta que se encuentra un breakpoint. En este caso se detuvo en el breakpoint de `main`, con el mensaje `Breakpoint 1, main () at codigo/03_gdb_basico.cpp:14`.

#### Qué hace `next`

Avanza hasta la siguiente línea del código. Si en la línea hay llamadas a otras funciones, estas se ejecutan completas sin detenerse dentro de ellas [4]. Por ejemplo, al hacer `next` en la línea 15, la función `duplicar` se ejecutó completa y `gdb` quedó detenido en la línea 16.

#### Qué hace `print`

Evalúa una expresión y muestra su valor [5]. Con `print numero` se muestra el valor de la variable `numero` en ese momento de la ejecución. Cada valor mostrado se guarda con una etiqueta como `$1`, `$2` o `$3`, en el orden en que se imprimieron.

### Salida de terminal del uso de gdb

```text
$ gdb ./gdb_basico
GNU gdb (Ubuntu 12.1-0ubuntu1~22.04.2) 12.1
...
Reading symbols from ./gdb_basico...
(gdb) break main
Breakpoint 1 at 0x120b: file codigo/03_gdb_basico.cpp, line 14.
(gdb) run
Starting program: /home/brandonfj/repos/ie0417-laboratorios-C12976/laboratorio-depuracion/gdb_basico
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".

Breakpoint 1, main () at codigo/03_gdb_basico.cpp:14
14          int numero = 5;
(gdb) next
15          int doble = duplicar(numero);
(gdb) next
16          int total = sumar(doble, 10);
(gdb) print numero
$1 = 5
(gdb) next
18          std::cout << "Número: " << numero << std::endl;
(gdb) print doble
$2 = 10
(gdb) next
Número: 5
19          std::cout << "Doble: " << doble << std::endl;
(gdb) print total
$3 = 20
(gdb) continue
Continuing.
Doble: 10
Total: 20
[Inferior 1 (process 28579) exited normally]
(gdb) quit
```

Al detenerse, `gdb` muestra la línea que está a punto de ejecutarse, y no la que ya se ejecutó. Por eso, cuando `gdb` indica la línea 16, las líneas 14 y 15 ya se ejecutaron y `numero` y `doble` ya tienen valor, pero `total` todavía no. Lo mismo ocurre con el orden de los mensajes. `Número: 5` apareció hasta después del `next` que ejecutó la línea 18, no antes.

### Valores observados

| Variable | Valor observado | Momento en que se imprimió |
|----------|-----------------|----------------------------|
| `numero` | 5               | Detenido en la línea 16    |
| `doble`  | 10              | Detenido en la línea 18    |
| `total`  | 20              | Detenido en la línea 19    |

Los valores coinciden con la ejecución normal (`Número: 5`, `Doble: 10`, `Total: 20`), lo que confirma que el programa funciona bien y que `gdb` solo permitió observarlo con más detalle.

---

## Preguntas de reflexión

**1. ¿Qué es un breakpoint?**

Un breakpoint es una pausa que se coloca en una parte del código. Cuando el programa se ejecuta con el depurador y llega a esa ubicación, se detiene justo antes de ejecutar esa instrucción [2]. Desde ahí se pueden revisar variables y continuar la ejecución.

**2. ¿Qué diferencia hay entre ejecutar el programa normalmente y ejecutarlo dentro de `gdb`?**

Con `gdb` se puede ejecutar el código poco a poco, detenerse exactamente en una línea y ver los valores de las variables y cómo cambian línea a línea. En cambio, en una ejecución normal el programa corre de principio a fin y solo se ve lo que imprime, es decir, el resultado final, sin poder analizar qué pasa en cada línea.

**3. ¿Qué ventaja tiene inspeccionar variables mientras el programa se ejecuta?**

Es muy útil cuando un programa falla o da un resultado incorrecto, porque se puede observar el código línea por línea e identificar con más facilidad en qué punto una variable toma un valor distinto al esperado. Esto es especialmente importante en los errores lógicos, que no producen ningún mensaje y solo se notan al comparar valores. Los errores de sintaxis, en cambio, ya los detecta el compilador antes de ejecutar.

**4. ¿Por qué `next` no entra dentro de las funciones?**

Porque `next` está pensado para avanzar de línea en línea dentro de la función actual, y cuando una línea contiene una llamada a otra función, la ejecuta completa y se detiene hasta la línea siguiente [4]. En el ejercicio se vio al hacer `next` en la línea 15. La función `duplicar` se ejecutó por completo y `gdb` quedó en la línea 16, sin mostrar lo que pasó dentro. Esto es práctico cuando ya se sabe que la función funciona bien y no interesa revisarla.

---

## Referencias

[1] GDB, "Compiling for Debugging", en Debugging with GDB. https://sourceware.org/gdb/current/onlinedocs/gdb.html/Compilation.html

[2] GDB, "Setting Breakpoints", en Debugging with GDB. https://sourceware.org/gdb/current/onlinedocs/gdb.html/Set-Breaks.html

[3] GDB, "Starting your Program", en Debugging with GDB. https://sourceware.org/gdb/current/onlinedocs/gdb.html/Starting.html

[4] GDB, "Continuing and Stepping", en Debugging with GDB. https://sourceware.org/gdb/current/onlinedocs/gdb.html/Continuing-and-Stepping.html

[5] GDB, "Examining Data", en Debugging with GDB. https://sourceware.org/gdb/current/onlinedocs/gdb.html/Data.html
