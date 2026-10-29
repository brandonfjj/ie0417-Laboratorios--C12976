# Pruebas unitarias

## Parte 2: implementación inicial del código

### Qué se hizo

Se crearon tres módulos en C++, cada uno con su encabezado en `include/` y su implementación en `src/`. Son las funciones que se probarán en las partes siguientes.

### Archivos creados

`include/calculator.h`, `src/calculator.cpp`, `include/string_utils.h`, `src/string_utils.cpp`, `include/grade_utils.h` y `src/grade_utils.cpp`. El contenido es el indicado en el enunciado

### Módulos

| Módulo | Encabezado (`.h`) | Implementación (`.cpp`) |
|---|---|---|
| `calculator` | Declara `add`, `subtract`, `multiply`, `divide` e `is_even` | Implementa las operaciones aritméticas. `divide` lanza `std::invalid_argument` si el divisor es cero |
| `string_utils` | Declara `to_uppercase`, `is_palindrome` y `count_vowels` | Convierte a mayúsculas, ignora espacios y mayúsculas para detectar palíndromos y cuenta las vocales |
| `grade_utils` | Declara `average`, `is_passing` y `letter_grade` | Calcula el promedio, decide si una nota aprueba (70 o más) y la convierte a letra. Lanza `std::invalid_argument` si la lista está vacía o la nota sale de 0 a 100 |

Los `.h` solo dicen qué funciones existen y cómo se llaman. Los `.cpp` contienen la lógica. Esa separación permite que las pruebas incluyan el encabezado sin conocer la implementación.

### Tipos de casos por función

| Función | Normal | Borde | Inválido |
|---|---|---|---|
| `add`, `subtract`, `multiply` | Enteros positivos | Negativos y cero | No aplica |
| `divide` | `10 / 2` | Negativos | Divisor 0 |
| `is_even` | `8` y `7` | 0 y negativos | No aplica |
| `to_uppercase` | `"hola"` | Cadena vacía y dígitos | No aplica |
| `is_palindrome` | `"radar"` | Cadena vacía, una letra y espacios | No aplica |
| `count_vowels` | `"murcielago"` | Cadena vacía y sin vocales | No aplica |
| `average` | `{80, 90, 100}` | Un solo elemento | Lista vacía |
| `is_passing` | `85` y `50` | `69` y `70` | No aplica |
| `letter_grade` | `95`, `85`, `75`, `65` y `50` | `0`, `100` y los cortes `59/60`, `69/70`, `79/80`, `89/90` | `-1` y `101` |

## Parte 3: primeras pruebas unitarias con Google Test

### Qué se hizo

Se escribieron las pruebas unitarias de los tres módulos y se compiló y ejecutó el proyecto completo por primera vez.

### Archivos creados

`tests/test_calculator.cpp` (8 pruebas), `tests/test_string_utils.cpp` (7 pruebas) y `tests/test_grade_utils.cpp` (10 pruebas). El contenido es el indicado en el enunciado.

### Comandos ejecutados

Desde `laboratorio-testing`:

```bash
mkdir build
cd build
cmake ..
make
./run_tests
ctest --output-on-failure
```

`cmake ..` descargó Google Test y `make` compiló sin errores.

### Resultado de `./run_tests`

Salida resumida, con `[...]` donde se omitieron las pruebas intermedias.

```
Running main() from .../build/_deps/googletest-src/googletest/src/gtest_main.cc
[==========] Running 25 tests from 3 test suites.
[----------] Global test environment set-up.
[----------] 8 tests from CalculatorTest
[ RUN      ] CalculatorTest.AddPositiveNumbers
[       OK ] CalculatorTest.AddPositiveNumbers (0 ms)
[...]
[----------] 10 tests from GradeUtilsTest (0 ms total)

[----------] Global test environment tear-down
[==========] 25 tests from 3 test suites ran. (1 ms total)
[  PASSED  ] 25 tests.
```

### Resultado de `ctest --output-on-failure`

Salida resumida.

```
Test project .../laboratorio-testing/build
      Start  1: CalculatorTest.AddPositiveNumbers
 1/25 Test  #1: CalculatorTest.AddPositiveNumbers ..................   Passed    0.00 sec
[...]
25/25 Test #25: GradeUtilsTest.InvalidGradeThrowsException .........   Passed    0.00 sec

100% tests passed, 0 tests failed out of 25

Total Test time (real) =   0.04 sec
```

### Cantidad de pruebas

| Pruebas ejecutadas | Exitosas | Fallidas |
|---|---|---|
| 25 | 25 | 0 |

### Explicación de cinco pruebas

**`CalculatorTest.DivideByZeroThrowsException`**

```cpp
EXPECT_THROW(divide(10, 0), std::invalid_argument);
```

Verifica que dividir entre cero no devuelva un valor, sino que lance `std::invalid_argument`. Es un caso inválido, y la prueba pasa solo si se lanza esa excepción.

**`CalculatorTest.DetectOddNumber`**

```cpp
EXPECT_FALSE(is_even(7));
```

Comprueba que un número impar no se reporte como par. Junto con `DetectEvenNumber` cubre los dos resultados posibles de `is_even`.

**`StringUtilsTest.DetectPalindromeWithSpaces`**

```cpp
EXPECT_TRUE(is_palindrome("anita lava la tina"));
```

La frase solo es palíndromo si se ignoran los espacios. La prueba asegura que `is_palindrome` los quite antes de comparar.

**`GradeUtilsTest.CalculateAverage`**

```cpp
std::vector<int> grades = {80, 90, 100};
EXPECT_DOUBLE_EQ(average(grades), 90.0);
```

Verifica el promedio de tres notas. Se usa `EXPECT_DOUBLE_EQ` y no `EXPECT_EQ` porque el resultado es un `double`, y este tipo de dato se compara con una tolerancia muy pequeña.

**`GradeUtilsTest.InvalidGradeThrowsException`**

```cpp
EXPECT_THROW(letter_grade(120), std::invalid_argument);
```

Una nota de 120 está fuera del rango de 0 a 100, así que la función debe rechazarla con una excepción en lugar de devolver una letra.

### Qué se aprendió

`ctest` y `./run_tests` ejecutan las mismas 25 pruebas. La diferencia es que `ctest` las corre una por una y las lista por nombre, mientras que `./run_tests` las corre juntas y muestra el detalle de Google Test.

## Preguntas de reflexión de la Parte 3

### 1. ¿Qué significa que una prueba pase?

Significa que al ejecutarse la prueba, no falle y que no exista un fallo del programa durante la prueba [1]. En la salida se ve como `[       OK ]` y, si pasan todas, como `[  PASSED  ] 25 tests.`

### 2. ¿Qué significa que una prueba falle?

Una prueba falla si alguna de sus corridas falla, sea fatal o no, o si el programa se cae durante la prueba [1]. Cuando eso pasa, Google Test muestra el archivo y la línea de la aserción junto con un mensaje [1], lo que ayuda a ubicar el error.

### 3. ¿Qué diferencia hay entre probar una función manualmente y probarla con Google Test?

Manualmente hay que ejecutar el programa, mirar la salida y juzgar si es correcta cada vez. Con Google Test el resultado esperado queda escrito en la prueba, así que la comparación la hace el programa y el resultado completo se obtiene con un solo comando. Además, `RUN_ALL_TESTS()` devuelve 0 si todo pasó y 1 si no [1], por lo que otras herramientas pueden usar ese resultado sin leer el texto.

### 4. ¿Por qué las pruebas unitarias deben ser rápidas?

Si el conjunto de pruebas tarda, se ejecuta con menos frecuencia y pierde valor, y hasta puede terminar omitiéndose [2]. Las 25 pruebas de este laboratorio corrieron en 1 ms, así que se pueden ejecutar después de cada cambio sin esfuerzo.

### 5. ¿Por qué las pruebas unitarias deben ser deterministas?

Una prueba que a veces pasa y a veces falla sin que el código cambie obliga a investigar fallas que no son reales, y hace que el equipo deje de confiar en el resultado, pues no se sabe con ciencia cierta que está pasando realmente. Si una prueba es determinista, una falla siempre indica que algo cambió en el código y se puede reproducir para corregirla.

## Parte 4: EXPECT vs ASSERT

### Qué se hizo

Se agregaron dos pruebas a `tests/test_calculator.cpp`, una con varias verificaciones `EXPECT_` y otra que usa `ASSERT_NE` antes de dividir. Luego se probó qué pasa cuando la condición del `ASSERT_` falla.

### Pruebas agregadas

```cpp
TEST(CalculatorTest, MultipleExpectChecks) {
    EXPECT_EQ(add(1, 1), 2);
    EXPECT_EQ(add(2, 2), 4);
    EXPECT_EQ(add(3, 3), 6);
}

TEST(CalculatorTest, AssertBeforeDivision) {
    int divisor = 2;

    ASSERT_NE(divisor, 0);

    EXPECT_EQ(divide(10, divisor), 5);
}
```

### Comandos ejecutados

```bash
cd build
make
./run_tests
```

### Resultado con `divisor = 2`

Ambas pruebas pasaron y el total subió a 27.

```
[ RUN      ] CalculatorTest.MultipleExpectChecks
[       OK ] CalculatorTest.MultipleExpectChecks (0 ms)
[ RUN      ] CalculatorTest.AssertBeforeDivision
[       OK ] CalculatorTest.AssertBeforeDivision (0 ms)
[...]
[==========] 27 tests from 3 test suites ran. (0 ms total)
[  PASSED  ] 27 tests.
```

### Actividad experimental con `divisor = 0`

Se cambió temporalmente `int divisor = 2;` por `int divisor = 0;` y se ejecutaron las pruebas.

```
[ RUN      ] CalculatorTest.AssertBeforeDivision
.../tests/test_calculator.cpp:45: Failure
Expected: (divisor) != (0), actual: 0 vs 0
[  FAILED  ] CalculatorTest.AssertBeforeDivision (0 ms)
[...]
[==========] 27 tests from 3 test suites ran. (0 ms total)
[  PASSED  ] 26 tests.
[  FAILED  ] 1 test, listed below:
[  FAILED  ] CalculatorTest.AssertBeforeDivision
```

Solo se reportó una falla, la de la línea 45. La línea `EXPECT_EQ(divide(10, divisor), 5)` no llegó a ejecutarse.

### Comparación con `EXPECT_NE`

Para ver la diferencia, se cambió también `ASSERT_NE` por `EXPECT_NE`, con `divisor = 0`. Esta vez la prueba siguió después de la verificación fallida y llegó a `divide(10, 0)`.

```
.../tests/test_calculator.cpp:45: Failure
Expected: (divisor) != (0), actual: 0 vs 0
unknown file: Failure
C++ exception with description "No se puede dividir entre cero" thrown in the test body.
[  FAILED  ] CalculatorTest.AssertBeforeDivision (0 ms)
```

Con `EXPECT_NE` aparecen dos fallas, y la segunda es una consecuencia de la primera. Después se devolvió la prueba a su versión correcta (`divisor = 2` y `ASSERT_NE`) y las 27 pruebas volvieron a pasar.

### Por qué `ASSERT_NE` detuvo la prueba

`ASSERT_` produce una falla fatal, que termina la función de prueba en el momento en que falla. Como el divisor valía 0, la prueba se detuvo en la línea 45 y nunca intentó dividir.

### Diferencia entre `EXPECT_` y `ASSERT_`

`EXPECT_` registra la falla pero deja que la prueba continúe, mientras que `ASSERT_` la termina de inmediato. Por eso se usa `ASSERT_` para condiciones sin las cuales no tiene sentido seguir.

### Qué se aprendió

Un `ASSERT_` bien colocado evita fallas en cadena que confunden. En la comparación con `EXPECT_NE`, el error real era el divisor en cero, pero la salida mostraba además una excepción que solo era consecuencia de ese primer error.

## Preguntas de reflexión de la Parte 4

### 1. ¿Cuándo conviene usar `EXPECT_`?

En la mayoria de veces, porque permite que una misma prueba reporte varias fallas en una sola ejecución [1]. En `MultipleExpectChecks`, si dos de las tres sumas estuvieran mal, se verían las dos de una vez.

### 2. ¿Cuándo conviene usar `ASSERT_`?

Cuando seguir no tiene sentido si la condición falla, como una precondición que las líneas siguientes necesitan. En `AssertBeforeDivision`, el divisor distinto de cero es lo que hace válida la división.

### 3. ¿Qué podría pasar si se usa `EXPECT_` cuando en realidad se necesitaba detener la prueba?

La prueba sigue con datos que ya se sabe que son incorrectos y produce fallas secundarias que no aportan en el reporte. Se vio en la comparación, donde con `EXPECT_NE` apareció una excepción que solo era consecuencia del divisor en cero, y había que leer con cuidado para notar cuál era la causa original.

### 4. ¿Qué podría pasar si se usa `ASSERT_` en exceso?

La prueba se detiene en la primera falla y las verificaciones siguientes no se ejecutan, así que habría que corregir y volver a correr varias veces para ir descubriendo los demás errores. Como `EXPECT_` permite ver más de una falla en una misma ejecución [1], es mejor reservar `ASSERT_` para las precondiciones.

## Referencias

[1] Google, "GoogleTest Primer", en GoogleTest User's Guide. https://google.github.io/googletest/primer.html

[2] Winters, T., Manshreck, T. y Wright, H., "Testing Overview", en Software Engineering at Google. https://abseil.io/resources/swe-book/html/ch11.html
