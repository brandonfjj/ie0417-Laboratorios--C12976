# Pruebas fallidas y corrección de errores

## Parte 5: pruebas fallidas y corrección de errores

### Qué se hizo

Se introdujo un error a propósito en la función `is_even` para ver cómo reaccionan las pruebas, se leyó el mensaje de Google Test y se corrigió la función.

### Cambio incorrecto realizado

En `src/calculator.cpp` se cambió la comparación de la función.

```cpp
bool is_even(int number) {
    return number % 2 == 1;
}
```

La versión correcta compara contra `0`. Con `== 1`, la función devuelve verdadero para los impares y falso para los pares, o sea, lo contrario de lo que debe.

### Comandos ejecutados

```bash
cd build
make
./run_tests
```

### Pruebas que fallaron

Fallaron dos pruebas, ambas de `is_even`. Las otras 25 siguieron pasando.

```
tests/test_calculator.cpp:29: Failure
Value of: is_even(8)
  Actual: false
Expected: true
[  FAILED  ] CalculatorTest.DetectEvenNumber (0 ms)
tests/test_calculator.cpp:33: Failure
Value of: is_even(7)
  Actual: true
Expected: false
[  FAILED  ] CalculatorTest.DetectOddNumber (0 ms)
[...]
[==========] 27 tests from 3 test suites ran. (0 ms total)
[  PASSED  ] 25 tests.
[  FAILED  ] 2 tests, listed below:
[  FAILED  ] CalculatorTest.DetectEvenNumber
[  FAILED  ] CalculatorTest.DetectOddNumber
```

`ctest` también reportó las dos fallas.

```
The following tests FAILED:
	  7 - CalculatorTest.DetectEvenNumber (Failed)
	  8 - CalculatorTest.DetectOddNumber (Failed)
```

### Qué esperaba la prueba y qué obtuvo

| Prueba | Expresión | Esperaba | Obtuvo |
|---|---|---|---|
| `DetectEvenNumber` | `is_even(8)` | `true` | `false` |
| `DetectOddNumber` | `is_even(7)` | `false` | `true` |

Con el error, `8 % 2` da `0`, que no es igual a `1`, y por eso la función devolvió `false`. Con `7 % 2` da `1`, y la función devolvió `true`.

### Corrección realizada

Se devolvió la comparación a su versión correcta.

```cpp
bool is_even(int number) {
    return number % 2 == 0;
}
```

### Resultado final

```
[==========] 27 tests from 3 test suites ran. (0 ms total)
[  PASSED  ] 27 tests.
```

`ctest` indicó `100% tests passed, 0 tests failed out of 27`.

### Qué se aprendió

El mensaje de Google Test dice el archivo, la línea, la expresión probada, el valor obtenido y el esperado, así que el error se ubica sin necesidad de depurar. También se notó que solo fallaron las pruebas relacionadas con la función dañada, lo que apunta directo a dónde está el problema.

## Preguntas de reflexión

### 1. ¿Cómo ayudó Google Test a identificar el error?

Mostró cuáles pruebas fallaron y, para cada una, la línea exacta y la comparación que no se cumplió. Como las dos pruebas fallidas llamaban a `is_even` y las demás pasaban, quedó claro que el problema estaba en esa función.

### 2. ¿Qué información útil muestra una prueba fallida?

Muestra el archivo y la línea de la aserción, la expresión evaluada, el valor real y el valor esperado. Con eso se puede saber qué se esperaba y qué pasó, sin tener que ejecutar el programa a mano.

### 3. ¿Por qué es importante ejecutar pruebas después de modificar código?

Porque permiten detectar errores rápidamente, y mientras más tarde se encuentra un error, más caro resulta corregir el mismo. Además, si las pruebas pasan, se puede seguir cambiando el software con más confianza.

### 4. ¿Qué riesgo existe si se cambia código y no se ejecutan las pruebas?

Que un error como el de `is_even` quede sin detectar y llegue más lejos, hasta que lo encuentre otra persona o el usuario. Un cambio que parece pequeño, como pasar de `0` a `1`, invierte por completo el resultado de la función.
