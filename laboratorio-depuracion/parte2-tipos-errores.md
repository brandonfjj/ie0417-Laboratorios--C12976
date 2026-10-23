# Parte 2: tipos de errores

## Objetivo

Identificar tres tipos comunes de errores:

- Errores de sintaxis.
- Errores de tiempo de ejecución.
- Errores lógicos.

---

## Ejercicio 1: error de sintaxis

### Código original

Archivo [`codigo/01_error_sintaxis.cpp`](codigo/01_error_sintaxis.cpp).

```cpp
#include <iostream>

int main() {
    std::cout << "Hola mundo" << std::endl
    return 0;
}
```

### Comando de compilación

```bash
g++ -g -o error_sintaxis codigo/01_error_sintaxis.cpp
```

### Mensaje de error original

```text
codigo/01_error_sintaxis.cpp: In function ‘int main()’:
codigo/01_error_sintaxis.cpp:4:43: error: expected ‘;’ before ‘return’
    4 |     std::cout << "Hola mundo" << std::endl
      |                                           ^
      |                                           ;
    5 |     return 0;
      |     ~~~~~~
```

### Línea donde estaba el error

La línea 4, en la columna 43, que corresponde al final de la instrucción con `std::cout`.

```cpp
    std::cout << "Hola mundo" << std::endl
```

### Causa del error

Faltaba el punto y coma al final de la línea. Toda instrucción en C++ debe terminar con `;`.

### Corrección

Se agregó el `;` al final de la línea 4.

### Código corregido

```cpp
#include <iostream>

int main() {
    std::cout << "Hola mundo" << std::endl;
    return 0;
}
```

### Resultado de la ejecución corregida

```bash
$ g++ -g -o error_sintaxis codigo/01_error_sintaxis.cpp
$ ./error_sintaxis
Hola mundo
```

### Preguntas de reflexión

**1. ¿Este error fue detectado antes o durante la ejecución?**

Fue detectado durante la compilación, antes de la ejecución. De hecho, como el compilador no pudo generar el ejecutable, el programa nunca llegó a correr.

**2. ¿Qué herramienta detectó el error?**

El compilador `g++`. Fue el encargado de revisar que el código cumpliera las reglas del lenguaje, y no hizo falta ninguna otra herramienta de depuración.

**3. ¿Por qué este tipo de error suele ser más fácil de corregir que un error lógico?**

Porque el compilador lo detecta de forma automática e indica el archivo, la línea y la posible causa, e incluso señala con `^` y `;` dónde debería ir lo que falta. En cambio, un error lógico no genera ningún aviso, y es necesario descubrirlo comparando el resultado con lo que se esperaba.

---

## Ejercicio 2: error lógico

### Código original

Archivo [`codigo/02_error_logico.cpp`](codigo/02_error_logico.cpp).

```cpp
#include <iostream>
#include <vector>

double calcular_promedio(const std::vector<int>& notas) {
    int suma = 0;

    for (int nota : notas) {
        suma += nota;
    }

    return suma;
}

int main() {
    std::vector<int> notas = {80, 90, 100};

    double promedio = calcular_promedio(notas);

    std::cout << "Promedio: " << promedio << std::endl;

    return 0;
}
```

### Comando de compilación

```bash
g++ -g -o error_logico codigo/02_error_logico.cpp
```

### Comando de ejecución

```bash
./error_logico
```

### Resultado obtenido originalmente

```text
Promedio: 270
```

### Resultado esperado

El promedio de 80, 90 y 100 se calcula a mano como (80 + 90 + 100) / 3 = 270 / 3 = 90.

```text
Promedio: 90
```

### Explicación del error

El programa compila y se ejecuta sin problemas, pero el resultado es incorrecto. La función `calcular_promedio` suma todas las notas, pero nunca divide esa suma entre la cantidad de notas, y devuelve directamente la suma (270). Es un error de quien escribió el código, que olvidó el último paso del cálculo del promedio.

### Corrección

Despues de ciclo for, se dividió la suma entre la cantidad de elementos del vector (`notas.size()`) antes de retornar. Por lo tanto, se agregó la linea: `suma = suma / notas.size();`.

### Código corregido

```cpp
#include <iostream>
#include <vector>

double calcular_promedio(const std::vector<int>& notas) {
    int suma = 0;

    for (int nota : notas) {
        suma += nota;
    }

    suma = suma / notas.size();
    return suma;
}

int main() {
    std::vector<int> notas = {80, 90, 100};

    double promedio = calcular_promedio(notas);

    std::cout << "Promedio: " << promedio << std::endl;

    return 0;
}
```

### Resultado final

```bash
$ g++ -std=c++17 -Wall -Wextra -g -o error_logico codigo/02_error_logico.cpp
$ ./error_logico
Promedio: 90
```


### Preguntas de reflexión

**1. ¿Por qué el compilador no detectó este error?**

Porque el código es válido para el lenguaje. No hay ningún error de sintaxis ni de tipos, y devolver la suma como si fuera el promedio es una instrucción perfectamente valida. El compilador solo verifica las reglas de C++, no tiene forma de saber que la intención del programador era calcular un promedio.

**2. ¿Por qué este error se considera lógico?**

Porque el problema está en la lógica del algoritmo y no en cómo está escrito el código. El programa hace exactamente lo que se le indicó, que es sumar y retornar, pero eso no es lo que se quería. No falla ni muestra mensajes de error, simplemente entrega un resultado incorrecto.

**3. ¿Qué estrategia usó para encontrarlo?**

Se calculó el promedio a mano para saber que el resultado correcto era 90, y se comparó contra el 270 que mostró el programa. Luego se revisó el código de la función `calcular_promedio` y se notó que faltaba dividir entre la cantidad de elementos del vector.

**4. ¿Qué pruebas adicionales podría hacer?**

Probar con otros conjuntos de notas cuyo promedio se conozca de antemano. También valdría la pena probar un vector con un solo elemento, con todas las notas iguales y con un vector vacío, que haría dividir entre cero y requeriría una validación.


