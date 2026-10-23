# Parte 1: instalación y verificación del ambiente

## Objetivo

Verificar que el ambiente de trabajo cuenta con las herramientas necesarias para compilar, ejecutar y depurar programas en C++.

## Sistema operativo

El laboratorio se realizó en WSL2 sobre Windows, con la distribución Ubuntu.

```bash
$ cat /etc/os-release | head -3
PRETTY_NAME="Ubuntu 22.04.5 LTS"
NAME="Ubuntu"
VERSION_ID="22.04"

$ uname -r
6.6.87.2-microsoft-standard-WSL2
```

## Versiones de las herramientas

### g++

```bash
$ g++ --version
g++ (Ubuntu 11.4.0-1ubuntu1~22.04.3) 11.4.0
Copyright (C) 2021 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

### gdb

```bash
$ gdb --version
GNU gdb (Ubuntu 12.1-0ubuntu1~22.04.2) 12.1
Copyright (C) 2022 Free Software Foundation, Inc.
```

### valgrind

```bash
$ valgrind --version
valgrind-3.18.1
```

## Programa de prueba

Archivo [`codigo/prueba.cpp`](codigo/prueba.cpp).

```cpp
#include <iostream>

int main() {
    std::cout << "Ambiente listo para depuración" << std::endl;
    return 0;
}
```

### Comando de compilación

```bash
g++ -g -o prueba codigo/prueba.cpp
```

La compilación terminó sin errores ni advertencias.

### Comando de ejecución y resultado

```bash
$ ./prueba
Ambiente listo para depuración
```

El programa imprimió el mensaje esperado y terminó con código de salida 0, lo que confirma que el compilador y el ambiente funcionan correctamente.


## Preguntas de reflexión

**1. ¿Para qué sirve `g++`?**

Sirve para compilar programas escritos en C++. Toma los archivos `.cpp` y los pasa por cuatro etapas, siempre en el mismo orden, que son el preprocesado, la compilación propiamente dicha, el ensamblado y el enlazado. Al final de ese proceso se obtiene un ejecutable [1]. La diferencia con `gcc` es que `g++` trata los archivos como C++ y enlaza automáticamente la biblioteca estándar de C++ [2]. También es el primer filtro de errores, porque cualquier error de sintaxis o de tipos se reporta en esta etapa.

**2. ¿Para qué sirve `gdb`?**

Sirve para observar lo que pasa dentro de un programa mientras se ejecuta, o lo que estaba haciendo en el momento en que se cayó [3]. Con `gdb` se puede iniciar el programa, hacer que se detenga en condiciones específicas, examinar lo que ocurrió cuando se detuvo y hasta cambiar valores para probar si una corrección funciona [3]. Es decir, `gdb` permite poner breakpoints, avanzar línea por línea, observar el valor de las variables en cualquier momento y ver qué cadena de funciones llevó a un punto del programa. Es útil cuando el programa falla o da resultados incorrectos y se quiere depurar para ver en donde está el problema.

**3. ¿Para qué sirve `valgrind`?**

`valgrind` sirve para analizar como usa la memoria un programa. Su herramienta principal, Memcheck, detecta memoria reservada que nunca se libera, lecturas o escrituras fuera de un bloque reservado, uso de memoria ya liberada, uso de variables sin inicializar y liberaciones incorrectas, como liberar dos veces el mismo bloque o mezclar `new[]` con `delete` [4]. Este tipo de errores muchas veces no hace que el programa falle de forma visible, por eso hace falta una herramienta que los busque.

**4. ¿Por qué se recomienda compilar con `-g` al depurar?**

Se recomienda pues la opción `-g` hace que el compilador genere información de depuración en el formato nativo del sistema, que en Linux es DWARF, y GDB puede trabajar con esta  información [5]. Esa información se guarda dentro del ejecutable y describe el tipo de cada variable y función, además de la correspondencia entre las líneas del código fuente y las direcciones del código máquina [6]. Gracias a eso `gdb` puede mostrar variables por su nombre y detenerse en una línea específica, y la documentación de GDB recomienda usar `-g` siempre que se compile [6]. Valgrind también lo recomienda, para que sus mensajes de error indiquen el número exacto de línea [7]. Sin esa información, las herramientas solo podrían mostrar direcciones de memoria y código ensamblador, lo que hace mucho más difícil ubicar el error en el código.

**5. ¿Qué diferencia hay entre compilar un programa y depurarlo?**

La diferencia es que compilar es traducir el código a un ejecutable, y solo garantiza que el código cumple las reglas del lenguaje. Depurar es cuando se buscan y se corrigen errores en el comportamiento del programa, ya sea porque falla durante la ejecución o porque no da los resultados que debería de dar. Un programa puede compilar sin problemas y aun así necesitar depuración.

## Referencias

[1] GCC, "Options Controlling the Kind of Output", en Using the GNU Compiler Collection. https://gcc.gnu.org/onlinedocs/gcc/Overall-Options.html

[2] GCC, "Compiling C++ Programs", en Using the GNU Compiler Collection. https://gcc.gnu.org/onlinedocs/gcc/Invoking-G_002b_002b.html

[3] GDB, "Summary of GDB", en Debugging with GDB. https://sourceware.org/gdb/current/onlinedocs/gdb.html/Summary.html

[4] Valgrind, "Memcheck: a memory error detector", en Valgrind User Manual. https://valgrind.org/docs/manual/mc-manual.html

[5] GCC, "Options for Debugging Your Program", en Using the GNU Compiler Collection. https://gcc.gnu.org/onlinedocs/gcc/Debugging-Options.html

[6] GDB, "Compiling for Debugging", en Debugging with GDB. https://sourceware.org/gdb/current/onlinedocs/gdb.html/Compilation.html

[7] Valgrind, "The Valgrind Quick Start Guide". https://valgrind.org/docs/manual/quick-start.html
