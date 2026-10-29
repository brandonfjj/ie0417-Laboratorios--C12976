# Casos borde

## Parte 6: diseño de casos borde

### Qué se hizo

Se agregaron 13 pruebas que verifican valores límite y casos especiales, además de los casos normales que ya existían.

### Archivos modificados

`tests/test_calculator.cpp`, `tests/test_string_utils.cpp` y `tests/test_grade_utils.cpp`. El contenido es el indicado en el enunciado.

### Comandos ejecutados

```bash
cd build
make
./run_tests
```

### Qué es un caso borde

Un caso borde usa un valor que está en el límite de lo que la función acepta, o justo a un lado de ese límite [1]. Por ejemplo, el mínimo y el máximo de un rango, o una entrada vacía.

### Casos borde agregados

| Archivo | Pruebas | Qué verifican |
|---|---|---|
| `test_calculator.cpp` | `DivideNegativeNumbers`, `DividePositiveByNegative` | La división con signos negativos |
| | `ZeroIsEven` | Que `0` cuente como par |
| `test_string_utils.cpp` | `EmptyStringToUppercase` | Convertir una cadena vacía |
| | `EmptyStringIsPalindrome`, `SingleLetterIsPalindrome` | Palíndromos de longitud 0 y 1 |
| `test_grade_utils.cpp` | `MinimumValidGrade`, `MaximumValidGrade` | Las notas `0` y `100` |
| | `NegativeGradeThrowsException` | La nota `-1`, justo debajo del rango |
| | `GradeBoundaryBetweenFAndD`, `...DAndC`, `...CAndB`, `...BAndA` | Los pares `59/60`, `69/70`, `79/80` y `89/90` |

### Por qué son importantes

Los errores en una condición suelen estar en el límite. Por ejemplo, escribir `>` en lugar de `>=` o `<` en lugar de `<=` no cambia el resultado para casi ningún valor, pero sí para el valor exacto del corte. Los casos normales no ayudan a detectar eso.

### Resultado de las pruebas

```
[==========] 40 tests from 3 test suites ran. (0 ms total)
[  PASSED  ] 40 tests.
```

`ctest` indicó `100% tests passed, 0 tests failed out of 40`.

### Pruebas fallidas

Ninguna prueba falló, así que no hubo nada que corregir en esta parte.

### Qué se aprendió

Que todas las pruebas de borde pasen confirma que los cortes de `letter_grade` están bien escritos. También se vio que con una sola nota normal por letra no se habría comprobado nada sobre los cortes.

## Preguntas de reflexión

### 1. ¿Por qué no basta con probar casos normales?

Porque los casos normales funcionan aunque el límite esté mal escrito. Una función puede devolver el resultado correcto para `85` y equivocarse con `80`, y solo una prueba que use esos valores en el límite lo detecta [1]. Además, probar con casos borde le da menos margen de error de que un programa falle.

### 2. ¿Qué es un caso borde?

Es una prueba con un valor en el borde de un rango válido o justo a un lado de él [1]. En este laboratorio son, por ejemplo, `0`, `100`, `59` y `60` para `letter_grade`, o la cadena vacía para `is_palindrome`.

### 3. ¿Qué es un caso inválido?

Es una entrada que la función no debe aceptar, y para la cual se espera un rechazo en lugar de un resultado normal. Por ejemplo, `letter_grade(-1)` o `letter_grade(120)` deben lanzar `std::invalid_argument`, y `divide(10, 0)` también.

### 4. ¿Qué diferencia hay entre probar `85` y probar exactamente `80`, `90` o `70`?

El `85` está lejos de cualquier corte, así que da el mismo resultado aunque la condición tenga un error de un solo valor. Por ejemplo, si el código dijera `grade > 80` en lugar de `grade >= 80`, el `85` seguiría dando `B`, pero el `80` daría `C`. Los valores exactos `70`, `80` y `90` sí exponen esos errores.

### 5. ¿Cómo puede un caso borde revelar errores ocultos?

Porque muchos errores solo se manifiestan en un valor concreto, y los casos normales casi nunca caen en él. Un caso borde obliga a ejecutar justo esa parte de la condición. Si `GradeBoundaryBetweenCAndB` usara el valor `80` y la función tuviera `>` en vez de `>=`, la prueba fallaría y mostraría el error que estaba escondido.

## Semillas en pruebas

### Qué se hizo

Se agregó a `tests/test_calculator.cpp` una prueba que suma 10 pares de números aleatorios y verifica que `add(a, b)` sea igual a `a + b`. Los números salen de un generador con semilla fija.

### Archivo modificado

`tests/test_calculator.cpp`, con `#include <random>` y esta prueba.

```cpp
TEST(CalculatorTest, RandomAdditionsWithFixedSeed) {
    std::mt19937 generator(12345);
    std::uniform_int_distribution<int> distribution(-100, 100);

    for (int i = 0; i < 10; i++) {
        int a = distribution(generator);
        int b = distribution(generator);

        EXPECT_EQ(add(a, b), a + b);
    }
}
```

### Comandos ejecutados

```bash
cd build
make
./run_tests
```

### Resultado con la semilla 12345

```
[ RUN      ] CalculatorTest.RandomAdditionsWithFixedSeed
[       OK ] CalculatorTest.RandomAdditionsWithFixedSeed (0 ms)
[...]
[==========] 41 tests from 3 test suites ran. (0 ms total)
[  PASSED  ] 41 tests.
```

### Qué pasó al cambiar la semilla

Se cambió temporalmente `std::mt19937 generator(12345);` por `std::mt19937 generator(54321);` y la prueba también pasó, porque `add` es correcta para cualquier par de números. Lo que cambió fueron los valores que se probaron. Para verlo, se usó un programa auxiliar aparte, con el mismo generador y la misma distribución, que imprime los primeros seis valores de cada semilla.

```
semilla 12345: 86 78 -37 -74 -64 -93
semilla 12345: 86 78 -37 -74 -64 -93
semilla 54321: 83 2 25 -64 59 62
```

Con la misma semilla se obtuvo la misma secuencia en las dos ejecuciones, y con otra semilla la secuencia fue distinta. Luego se devolvió la semilla a `12345` y las 41 pruebas pasaron.

### Qué hace la semilla

La semilla define el estado inicial del generador [2]. Como el generador es un algoritmo y no una fuente de azar real, a partir de una misma semilla siempre produce los mismos números en el mismo orden.

### Por qué se usa una semilla fija

Para que la prueba sea determinista. Con `12345` la prueba siempre usa los mismos 20 números, así que su resultado no cambia de una ejecución a otra.

### Por qué ayuda en debugging

Si una prueba con datos aleatorios fallara, bastaría con volver a ejecutarla con la misma semilla para tener exactamente los mismos datos y poder estudiar la falla. Sin una semilla fija, el valor que causó el error se perdería en cuanto termine la ejecución.

### Qué se aprendió

Que "aleatorio" en una prueba no significa "impredecible". Con una semilla fija se mantiene la variedad de datos sin perder la repetibilidad.

## Preguntas de reflexión de la Parte 7

### 1. ¿Por qué las pruebas con datos aleatorios pueden ser peligrosas si no se controlan?

Porque pueden pasar en una ejecución y fallar en otra sin que el código haya cambiado, y entonces no hay forma de saber con qué datos ocurrió la falla. Eso las vuelve difíciles de reproducir y de corregir.

### 2. ¿Qué ventaja tiene usar una semilla fija?

Los datos son siempre los mismos, así que la prueba es repetible y determinista. Los generadores de la biblioteca estándar de C++ están pensados para poder sembrarse y usarse en simulaciones repetibles [2].

### 3. ¿Cómo ayuda la semilla a reproducir errores?

Con la misma semilla se genera la misma secuencia de números, como se vio al imprimir los valores de `12345` dos veces. Si un error aparece con ciertos datos, se vuelve a ejecutar con esa semilla y aparecen otra vez los mismos datos.

### 4. ¿Por qué es importante documentar la semilla usada?

Porque sin saber qué semilla se usó no se puede reproducir la ejecución. Si una prueba falla en otro equipo o en el servidor de integración continua, la semilla es lo que permite volver a generar los mismos datos y buscar la causa.

## Referencias

[1] ISTQB, "Boundary value", en ISTQB Glossary. https://istqb-glossary.page/boundary-value/

[2] cppreference.com, "Pseudo-random number generation", en C++ reference. https://en.cppreference.com/w/cpp/numeric/random
