# Mini reto y reflexión final

## Parte 12: mini reto de testing

## Mini reto de testing

### Qué se hizo

Se agregó una función nueva, `is_valid_grade`, y se diseñaron seis pruebas para ella. Luego se compilaron y ejecutaron en local, y se subieron a GitHub en una rama aparte, `Laboratorio-testing-mini-reto`, con su propio pull request hacia `main`.

### Función implementada

`is_valid_grade` indica si una nota está dentro del rango permitido, de 0 a 100 incluidos.

```cpp
bool is_valid_grade(int grade) {
    return grade >= 0 && grade <= 100;
}
```

Se modificaron `include/grade_utils.h`, donde se agregó la declaración, y `src/grade_utils.cpp`, donde se agregó la implementación.

### Casos de prueba diseñados

Se agregaron en `tests/test_grade_utils.cpp`.

| Prueba | Valor | Resultado esperado |
|---|---|---|
| `ValidGradeNormalValue` | 85 | `true` |
| `InvalidGradeNormalValue` | 250 | `false` |
| `ValidGradeLowerBound` | 0 | `true` |
| `ValidGradeUpperBound` | 100 | `true` |
| `GradeJustBelowLowerBoundIsInvalid` | -1 | `false` |
| `GradeJustAboveUpperBoundIsInvalid` | 101 | `false` |

### Por qué se escogieron esos casos

Los dos casos normales comprueban el comportamiento general, uno dentro del rango y otro claramente fuera. Para el inválido se usó 250 y no un valor como 101, para que esta prueba no repita lo que ya cubre la del borde superior. Los otros cuatro rodean los dos límites, porque ahí es donde más fácil se comete un error en una comparación. Para cada límite se probó el último valor válido y el primer valor inválido.

### Comandos ejecutados

Se ejecutaron desde `laboratorio-testing/`.

```bash
cd build
make
./run_tests
```

### Resultado de las pruebas

Todas las pruebas pasaron en local, las 42 que ya existían más las 6 nuevas.

```
[ RUN      ] GradeUtilsTest.ValidGradeNormalValue
[       OK ] GradeUtilsTest.ValidGradeNormalValue (0 ms)
[ RUN      ] GradeUtilsTest.InvalidGradeNormalValue
[       OK ] GradeUtilsTest.InvalidGradeNormalValue (0 ms)
[ RUN      ] GradeUtilsTest.ValidGradeLowerBound
[       OK ] GradeUtilsTest.ValidGradeLowerBound (0 ms)
[ RUN      ] GradeUtilsTest.ValidGradeUpperBound
[       OK ] GradeUtilsTest.ValidGradeUpperBound (0 ms)
[ RUN      ] GradeUtilsTest.GradeJustBelowLowerBoundIsInvalid
[       OK ] GradeUtilsTest.GradeJustBelowLowerBoundIsInvalid (0 ms)
[ RUN      ] GradeUtilsTest.GradeJustAboveUpperBoundIsInvalid
[       OK ] GradeUtilsTest.GradeJustAboveUpperBoundIsInvalid (0 ms)
[  PASSED  ] 48 tests.
```

### Subida a GitHub

Sí. Se hizo un commit con los tres archivos modificados y se subió la rama `Laboratorio-testing-mini-reto`. Como el workflow solo escucha eventos de `main`, se abrió un pull request hacia `main` para que se ejecutara.

### Ejecución de GitHub Actions

GitHub Actions ejecutó el workflow sobre el pull request y terminó con estado exitoso. Todos los pasos salieron en verde.

![Workflow exitoso del mini reto](Captura_parte_12.png)

### Qué se aprendió

Que diseñar buenas pruebas es decidir qué valores vale la pena probar. Con seis casos bien elegidos, sobre todo los de los bordes, se cubre el comportamiento de la función sin necesidad de probar muchos valores.

## Preguntas de reflexión de la Parte 12

### 1. ¿Cuál fue el caso más obvio de probar?

Una nota válida común, como 85. Es el uso normal de la función y es lo primero que se piensa al probarla.

### 2. ¿Cuál fue el caso borde más importante?

Los dos valores que quedan justo en el límite, 0 y 100, porque son los que separan una nota válida de una inválida. Si el límite está mal escrito, es ahí donde se nota.

### 3. ¿Qué error podría aparecer si no se prueban los valores `0` y `100`?

Que la función use `>` y `<` en lugar de `>=` y `<=`. Con 85 y 250 la función seguiría dando el resultado esperado, pero rechazaría por error un 0 o un 100, que sí son notas válidas. Ese error pasaría desapercibido.

### 4. ¿Qué diferencia hay entre probar un valor como `50` y probar valores como `-1`, `0`, `100` y `101`?

Un valor como 50 está en medio del rango y casi cualquier implementación lo trata bien, por eso no dice mucho. Los valores -1, 0, 100 y 101 están pegados a los límites, donde una condición mal escrita da un resultado distinto. Por eso detectan errores que el 50 no detecta.

### 5. ¿Qué aprendió sobre diseñar pruebas?

Que no se trata de probar muchos valores, sino de probar especificos para probar el funcionamiento. Es decir, conviene pensar primero en donde podría fallar la función y escoger los casos a partir de eso, con un caso normal y los valores alrededor de cada límite.

## Parte 13: reflexión final

## Reflexión final

### 1. ¿Qué es software testing?

El software testing consiste en la actividad de ejecutar un programa con entradas definidas y comparar el resultado obtenido con el esperado, para detectar errores del mismo. En este laboratorio se hizo con pruebas escritas en código, que se ejecutan con un comando y dicen cuáles pasan y cuáles fallan.

### 2. ¿Por qué el testing mejora la calidad del software?

Porque esto permite encontrar errores antes de que lleguen a los usuarios que van a usar los programas, y mientras más temprano se detecta un error, más barato resulta corregirlo. Además, las pruebas dan confianza para modificar el código, ya que si algo se rompe, se nota de inmediato.

### 3. ¿Cuál es la diferencia entre verificación y validación?

La verificación comprueba que el producto cumple lo que se especificó, es decir, si se construyó bien. La validación comprueba que el producto sirve para el uso que se necesita, es decir, si se construyó lo correcto [1]. Las pruebas de este laboratorio son sobre todo verificación, porque revisan que las funciones se comporten como se definió.

### 4. ¿Qué es una prueba unitaria?

Es una prueba que revisa una pieza individual del software, como una función [2]. En este laboratorio, por ejemplo, `add(2, 3)` se probó por separado, sin depender del resto del programa.

### 5. ¿Qué es una prueba funcional?

Es una prueba cuyos casos se obtienen de lo que el sistema debe hacer según su especificación [3]. En la Parte 9 se hizo una tabla de casos a partir del requisito de convertir notas a letras, y se revisó que cada caso tuviera su prueba.

### 6. ¿Qué diferencia hay entre `EXPECT_` y `ASSERT_`?

Las dos comprueban una condición, pero en el caso de `ASSERT_`, el mismo detiene la prueba actual cuando falla, y `EXPECT_` registra el fallo pero deja que el programa continue. Por eso se usa `EXPECT_` en la mayoría de los casos, ya que reporta varios fallos en una sola ejecución. `ASSERT_` se deja para cuando seguir no tiene sentido.

### 7. ¿Por qué las pruebas deben ser deterministas?

Porque una prueba que a veces pasa y a veces falla sin que el código cambie hace perder la confianza en las pruebas, además de que perdería el sentido de probarla. Cuando eso ocurre, nadie sabe si un fallo es un error real o ruido.

### 8. ¿Por qué puede ser útil una semilla en pruebas con valores aleatorios?

Porque con una semilla fija el generador produce siempre la misma secuencia de valores. Así la prueba es repetible, y si falla, se puede volver a ejecutar con los mismos datos para encontrar la causa. Esto se vio en la Parte 7, donde cambiar la semilla cambió los valores de la ejecución.

### 9. ¿Qué es cobertura de código?

Es una métrica que indica qué partes del código se ejecutaron mientras corrían las pruebas. Se puede medir por líneas, funciones o ramas. En la Parte 8 se obtuvo 100% de líneas y funciones, y 87.5% de ramas.

### 10. ¿Por qué una cobertura alta no garantiza ausencia de errores?

Porque la cobertura como tal solo indica que tantas lineas se ejecutaron, no que el resultado se haya comprobado ni que el valor obtenido haya sido el esperado. Una prueba puede recorrer todo el código y aun así no revisar nada importante. En este laboratorio las líneas llegaron a 100%, pero quedaron ramas sin cubrir.

### 11. ¿Qué ventaja tiene ejecutar pruebas en GitHub Actions?

Las pruebas se ejecutan solas con cada cambio, en una máquina limpia y sin depender del equipo de nadie. En la Parte 11, un fallo intencional quedó marcado en el pull request apenas se subió, y al corregirlo el workflow volvió a salir en verde.

### 12. ¿Qué parte del laboratorio le pareció más útil?

Los casos borde, junto con la ejecución en GitHub Actions. Los casos borde mostraron que probar solo valores normales deja pasar errores en los límites, y en realidad estos casos son los que a veces se debe de prestar más atención para evitar probelmas en los programas. Actions mostró cómo una prueba que falla se detecta sin que nadie la ejecute a mano, lo cual resulta muy util.

### 13. ¿Qué parte le pareció más difícil?

Configurar GitHub Actions, porque el workflow solo funciona si está en la raíz del repositorio y solo se ejecuta con los eventos correctos. Al principio el push a la rama de trabajo no lo activaba, y fue necesario abrir un pull request hacia `main`.

### 14. ¿Cómo aplicaría pruebas automatizadas en un proyecto futuro?

Se escribirían pruebas junto con el código desde el inicio, incluyendo casos normales, casos borde y casos inválidos. Se ejecutarían en CI con cada pull request, para que nada llegue a la rama principal con pruebas fallidas. También se revisaría la cobertura, sabiendo que sirve para encontrar código sin probar y no para asegurar que no hay errores.

## Referencias

[1] ISTQB, "Verification", en ISTQB Glossary. https://istqb-glossary.page/verification/

[2] ISTQB, "Unit testing", en ISTQB Glossary. https://istqb-glossary.page/unit-testing/

[3] ISTQB, "Functional testing", en ISTQB Glossary. https://istqb-glossary.page/functional-testing/
