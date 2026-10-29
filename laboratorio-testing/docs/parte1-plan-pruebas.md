# Parte 1: preparación del proyecto

### Qué se hizo

Se crearon las carpetas del proyecto dentro de `laboratorio-testing`. Se usó una sola invocación de `mkdir -p` ejecutada desde esa carpeta, lo cual da el mismo resultado que las cinco líneas del enunciado.

### Comando ejecutado

```bash
mkdir -p include src tests docs .github/workflows
```

### Resultado

```bash
$ ls -a
.  ..  .github  docs  include  src  tests
```

### Qué se aprendió

La opción `-p` crea las carpetas intermedias, como `.github/` antes de `workflows/`, y no falla si ya existen. Eso permite repetir el comando sin riesgo.

## Archivo CMakeLists.txt

### Qué se hizo

Se creó el archivo `CMakeLists.txt` en la raíz de `laboratorio-testing` con el contenido indicado en el enunciado.

### Archivo creado

`CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.14)

project(LaboratorioTesting)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Opciones de cobertura
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

if(ENABLE_COVERAGE)
    message(STATUS "Coverage enabled")
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/release-1.12.1.zip
)

# Evita problemas en Windows con Google Test
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

include_directories(include)

add_library(project_lib
    src/calculator.cpp
    src/string_utils.cpp
    src/grade_utils.cpp
)

add_executable(run_tests
    tests/test_calculator.cpp
    tests/test_string_utils.cpp
    tests/test_grade_utils.cpp
)

target_link_libraries(run_tests
    project_lib
    gtest
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(run_tests)
```

## Conceptos de la parte

### Qué es CMake

CMake es un generador de sistemas de compilación multiplataforma [1]. No compila el programa por sí mismo. A partir de los archivos `CMakeLists.txt` genera los archivos nativos de compilación, por ejemplo un `Makefile` para usar con `make` o un proyecto para un IDE [1]. En este laboratorio se genera un `Makefile` y luego `make` hace la compilación.

### Para qué sirve Google Test

Google Test es un marco para escribir pruebas en C++ [2]. Ofrece macros como `TEST`, `EXPECT_EQ` y `ASSERT_EQ`, agrupa las pruebas en suites y reporta cuales pasaron y cuales fallaron [2]. Con él, cada comportamiento esperado queda escrito como código que se puede ejecutar.

### Qué significa automatizar pruebas

Automatizar pruebas significa que la verificación de un programa, la hace un programa de forma automática y no una persona. Las pruebas quedan escritas una vez, se ejecutan con un comando y el resultado dice que pruebas fallan y cuales pasan, sin que nadie tenga que revisar salidas a ojo.

### Qué significa que las pruebas sean repetibles

Una prueba repetible da el mismo resultado cada vez que se ejecuta bajo las mismas condiciones. Google Test recomienda que las pruebas sean independientes y repetibles, y por eso crea un objeto nuevo para cada prueba cuando se usan fixtures, de modo que lo que haga una no afecte a las demás [2]. Si el resultado dependiera del orden, del reloj o de datos aleatorios no controlados, una falla sería casi imposible de reproducir y depurar.

## Preguntas de reflexión

### 1. ¿Por qué conviene separar el código fuente de las pruebas?

El código fuente es lo que se entrega, y las pruebas solo existen para verificar el correcto funcionamiento. Si estuvieran mezcladas, el programa final tendría código de prueba no necesario, y sería más difícil saber qué es qué.

### 2. ¿Qué ventaja tiene usar CMake en un proyecto de C++?

CMake describe el proyecto una sola vez y genera los archivos de compilación para cada plataforma o herramienta [1]. Evita escribir y mantener un `Makefile` a mano, y además resuelve dependencias como Google Test con `FetchContent` [3].

### 3. ¿Por qué es útil que las pruebas se puedan ejecutar con un solo comando?

Porque se ejecutarán muchas veces, después de cada cambio. Si correrlas exigiera varios pasos manuales se volvería muy tedioso y tarde o temprano se omitirían. Con un solo comando como `ctest` o `./run_tests` [3] cualquier persona, o un servidor de integración continua, obtiene el mismo resultado sin conocer los detalles del proyecto.

### 4. ¿Qué diferencia hay entre probar manualmente y probar automáticamente?

Probar manualmente implica ejecutar el programa, mirar la salida y decidir si es correcta. Es lento, depende de la atención de quien prueba y no deja registro de lo que se verificó. Probar automáticamente deja los casos escritos como código con el resultado esperado. Se pueden repetir en segundos, son repetibles [2] y detectan de inmediato si un cambio nuevo rompió algo que antes funcionaba.

## Parte 9: pruebas funcionales sencillas

### Qué se hizo

Se tomó el requisito del enunciado sobre la conversión de notas a letras y se escribió una tabla de casos funcionales. Luego se revisó `tests/test_grade_utils.cpp` para ver cuáles casos ya tenían una prueba. Faltaba una y se agregó.

### Requisito

> El sistema debe convertir una nota numérica entre 0 y 100 a una letra de calificación. Las notas entre 90 y 100 son A, entre 80 y 89 son B, entre 70 y 79 son C, entre 60 y 69 son D, y menores a 60 son F. Si la nota está fuera del rango permitido, el sistema debe rechazarla.

### Tabla de casos funcionales

| ID | Requisito | Entrada | Resultado esperado | Tipo de caso |
|---|---|---:|---|---|
| TC-001 | Convertir nota excelente | 95 | A | Normal |
| TC-002 | Límite inferior de A | 90 | A | Borde |
| TC-003 | Límite superior de B | 89 | B | Borde |
| TC-004 | Nota inválida baja | -1 | Excepción | Inválido |
| TC-005 | Nota inválida alta | 101 | Excepción | Inválido |

### Cuáles ya estaban cubiertos

| ID | Prueba en `tests/test_grade_utils.cpp` |
|---|---|
| TC-001 | `LetterGradeA`, que prueba `95` |
| TC-002 | `GradeBoundaryBetweenBAndA`, que prueba `90` |
| TC-003 | `GradeBoundaryBetweenBAndA`, que prueba `89` |
| TC-004 | `NegativeGradeThrowsException`, que prueba `-1` |

### Cuáles se agregaron

TC-005 no tenía prueba. La única prueba de nota alta, `InvalidGradeThrowsException`, usa `120`, y el caso pide `101`, que es el valor justo encima del límite. Se agregó esta prueba en `tests/test_grade_utils.cpp`.

```cpp
TEST(GradeUtilsTest, GradeJustAboveMaximumThrowsException) {
    EXPECT_THROW(letter_grade(101), std::invalid_argument);
}
```

### Comandos ejecutados

```bash
cd build
make
./run_tests
```

### Resultado

Las pruebas que cubren los cinco casos pasaron.

```
[       OK ] GradeUtilsTest.LetterGradeA (0 ms)
[       OK ] GradeUtilsTest.NegativeGradeThrowsException (0 ms)
[       OK ] GradeUtilsTest.GradeJustAboveMaximumThrowsException (0 ms)
[       OK ] GradeUtilsTest.GradeBoundaryBetweenBAndA (0 ms)
```

Con la prueba nueva, el total pasó de 41 a 42.

```
[==========] 42 tests from 3 test suites ran. (0 ms total)
[  PASSED  ] 42 tests.
```

### Por qué son pruebas funcionales

Porque cada caso sale de lo que dice el requisito y no de cómo está escrito `letter_grade`. Las entradas y los resultados esperados se pudieron definir sin mirar el código, solo leyendo qué letra corresponde a cada rango y qué notas se deben rechazar [4]. En la tabla no aparece ningún `if` ni ningún `>=`, solo el comportamiento que el usuario espera.

### Qué se aprendió

Que una tabla hecha a partir del requisito sirve como lista de verificación. Al compararla con las pruebas existentes se vio que `101` no estaba probado, aunque ya existía una prueba para notas altas.

## Preguntas de reflexión de la Parte 9

### 1. ¿Qué diferencia hay entre una prueba unitaria y una prueba funcional?

Una prueba unitaria verifica una pieza individual del software, como una función [5]. Una prueba funcional se basa en la especificación de lo que el sistema debe hacer [4]. Las dos pueden terminar siendo el mismo código en este laboratorio, pero la unitaria se define por el tamaño de lo que se prueba y la funcional por el origen de los casos.

### 2. ¿Por qué una prueba funcional se relaciona con requisitos?

Porque los casos se derivan de la especificación [4]. Cada fila de la tabla remite a una parte del requisito, por ejemplo TC-005 al rechazo de notas fuera del rango. Si un requisito no tiene ningún caso, no hay forma de saber si se cumple.

### 3. ¿Qué significa pensar desde la perspectiva del usuario o del sistema?

Significa preguntarse qué espera recibir quien usa la función, sin fijarse en cómo está implementada. Quien ingresa un `90` espera una `A`, sin importar si el código compara con `>` o con `>=`. Pensar así ayuda a escribir pruebas que seguirían siendo válidas aunque se reescriba el código interno.

### 4. ¿Por qué la documentación de casos de prueba ayuda al equipo?

Porque deja escrito qué se verifica y por qué, y cualquier persona puede revisar si falta algo. Eso fue lo que pasó con TC-005, ya que al comparar la tabla con las pruebas se encontró un caso sin cubrir. También permite saber qué requisito se rompió cuando una prueba falla.

## Referencias

[1] CMake, "cmake(1)", en CMake Reference Documentation 3.22. https://cmake.org/cmake/help/v3.22/manual/cmake.1.html

[2] Google, "GoogleTest Primer", en GoogleTest User's Guide. https://google.github.io/googletest/primer.html

[3] Google, "Quickstart: Building with CMake", en GoogleTest User's Guide. https://google.github.io/googletest/quickstart-cmake.html

[4] ISTQB, "Functional testing", en ISTQB Glossary. https://istqb-glossary.page/functional-testing/

[5] ISTQB, "Component testing", en ISTQB Glossary. https://istqb-glossary.page/component-testing/
