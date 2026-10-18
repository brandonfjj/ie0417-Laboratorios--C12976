# Análisis del laboratorio de concurrencia

## Parte 1: creación básica de hilos

Ejecuté `./parte1` seis veces. Resumen del orden en que aparecieron los hilos:

### Resultados

| Ejecución | Orden observado | Observación |
|-----------|-----------------|-------------|
| 1 | Mensajes mezclados (se ven ids como `31`, `24`, `0` y textos cortados) | Salida entrelazada, algunos ids se pegaron entre sí |
| 2 | 0, 1, 2, 4, 3 | Líneas limpias |
| 3 | 0, 4, 2, 3, 1 | Un par de líneas se mezclaron al inicio |
| 4 | 4, 2, 1, 3, 0 | Una línea se cortó a la mitad |
| 5 | 0, 1, 2, 4, 3 | Líneas limpias |
| 6 | 3/4 (mezclados), 1, 2, 0 | Los ids 3 y 4 salieron pegados |

### 1. ¿Los mensajes aparecen siempre en el mismo orden?

No. Aunque los hilos se crean en el orden 0, 1, 2, 3, 4, el orden de los mensajes cambió de una ejecución a otra (por ejemplo, en la ejecución 4 salió primero el hilo 4 y al final el 0). Solo se repitió el mismo orden en las ejecuciones 2 y 5 (0, 1, 2, 4, 3), pero fue casualidad y no algo garantizado. Además, en varias ejecuciones los mensajes salieron mezclados entre sí, porque varios hilos escribían en `std::cout` al mismo tiempo.

### 2. ¿Por qué podría cambiar el orden de impresión?

Porque una vez que se crea un hilo, es el planificador (scheduler) del sistema operativo quien decide cuándo y en qué núcleo se ejecuta, y eso no depende del orden en que yo los creé. Crear el hilo 0 antes que el 1 no garantiza que el 0 llegue primero a imprimir. Además, el tiempo de arranque de cada hilo varía según la carga del sistema en ese momento. Los mensajes entrelazados que vi se deben a que `std::cout` no está protegido con ningún mutex en mi programa: cada `<<` es una operación aparte, entonces un hilo puede imprimir "Hola, soy el hilo " y otro hilo meterse antes de que el primero imprima su id. El programa no tiene un error de memoria, pero la salida no está sincronizada.

### 3. ¿Qué función cumple `join()`?

`join()` hace que el hilo que lo llama (aquí el hilo principal, `main`) se bloquee hasta que el hilo al que se le aplica termine su ejecución. Así `main` espera a que todos los hilos acaben antes de imprimir "Todos los hilos finalizaron." y de terminar el programa. También libera los recursos asociados al hilo: un `std::thread` tiene que ser "joined" (o "detached") antes de destruirse.

Según cppreference, `join()` "bloquea el hilo actual hasta que el hilo identificado por `*this` termina su ejecución" y, al retornar, `joinable()` pasa a ser `false` [1].

### 4. ¿Qué podría pasar si no se llama a `join()`?

Si un `std::thread` se destruye mientras sigue siendo "joinable" (sin haber llamado a `join()` ni `detach()`), el programa llama a `std::terminate` y se aborta. Aparte de eso, si `main` no esperara a los hilos y terminara antes, el programa podría cerrarse mientras los hilos aún no han impreso (o ni siquiera han arrancado), así que se perderían mensajes y no se podría asegurar que el trabajo se completó. Incluso el mensaje "Todos los hilos finalizaron." podría salir antes que los mensajes de los hilos, lo cual sería falso.

cppreference indica que si un `std::thread` con un hilo asociado (`joinable() == true`) se destruye, se llama a `std::terminate()`; solo es seguro destruirlo si ya se llamó a `join()` o `detach()`, o si fue movido o construido por defecto [2].

## Parte 2: condición de carrera

Compilé con `g++ -std=c++17 -Wall -Wextra -pthread src/parte2_race_condition.cpp -o parte2` (sin warnings) y ejecuté `./parte2` cinco veces. El programa usa 4 hilos y cada uno hace 1,000,000 incrementos, por eso el valor esperado es 4,000,000. La máquina tiene 20 núcleos lógicos según `nproc`.

### Resultados

| Ejecución | Valor esperado | Valor obtenido |
|-----------|----------------|----------------|
| 1 | 4000000 | 1077064 |
| 2 | 4000000 | 1135545 |
| 3 | 4000000 | 1076309 |
| 4 | 4000000 | 1087173 |
| 5 | 4000000 | 1075562 |

### 1. ¿El valor obtenido siempre coincide con el valor esperado?

No, en ninguna de las cinco ejecuciones coincidió con el valor esperado. El valor de 4000000 no se obtuvo en ninguna de las ejecuciones, lo que se estuvo estaba entre 1,075,562 y 1,135,545. Además, el valor cambió en cada ejecución, así que el error no solo existe sino que tampoco es predecible. Un programa correcto tendría que dar siempre 4,000,000.

### 2. ¿Por qué se pierden incrementos?

Porque los cuatro hilos leen y escriben la misma variable `contador` sin ningún tipo de coordinación. Un incremento se pierde cuando dos hilos leen el mismo valor antes de que alguno escriba el resultado. Por ejemplo, si `contador` vale 41 y dos hilos lo leen, ambos calculan 42 y ambos escriben 42, es decir, se ejecutaron dos incrementos, pero el contador solo subió una vez [5]. Como hay cuatro hilos haciendo esto un millón de veces cada uno, ese tipo de empalmes ocurre muchísimas veces y el resultado final queda muy por debajo de lo esperado.

Como la máquina tiene 20 núcleos, los hilos pueden correr realmente en paralelo en núcleos distintos, y no hace falta que el sistema operativo los interrumpa para que pase esto. El orden exacto de los empalmes depende del planificador y de la carga del momento, y por eso el resultado cambia de una ejecución a otra.

### 3. ¿La operación `contador++` es realmente una sola operación a nivel de CPU?

No. Aunque en C++ se escribe en una sola línea, la CPU la ejecuta en tres pasos: leer `contador` de memoria a un registro, sumarle 1 y escribir el resultado de vuelta en memoria [5]. En el ensamblador generado para este programa (sin `-O2`) se ven esas tres instrucciones dentro del ciclo de `incrementar`: un `mov` para leer, un `add` y otro `mov` para escribir. Como son pasos separados, otro hilo puede intervenir entre ellos, por eso la operación no es atómica.

### 4. ¿Qué problema de concurrencia se está observando?

Una condición de carrera, en concreto una carrera de datos, pues los cuatro hilos modifican la misma variable `contador` al mismo tiempo, sin sincronización y sin que la operación sea atómica [3]. Esto provoca los incrementos perdidos y, según el estándar de C++, hace que el comportamiento del programa sea indefinido [3]. Se corrige protegiendo `contador` con un `std::mutex` o usando `std::atomic<int>` [4].

## Parte 3: corrección usando mutex

El programa se compiló con `g++ -std=c++17 -Wall -Wextra -pthread src/parte3_mutex.cpp -o parte3` (sin warnings) y se ejecutó `./parte3` cinco veces. Es el mismo programa de la Parte 2 con 4 hilos y 1,000,000 incrementos por hilo, solo que cada incremento está protegido con un `std::mutex`.

### Resultados

| Ejecución | Valor esperado | Valor obtenido |
|-----------|----------------|----------------|
| 1 | 4000000 | 4000000 |
| 2 | 4000000 | 4000000 |
| 3 | 4000000 | 4000000 |
| 4 | 4000000 | 4000000 |
| 5 | 4000000 | 4000000 |

### Comparación con la Parte 2

| Parte | Valores obtenidos en las 5 ejecuciones | ¿Coincide con el esperado? |
|-------|----------------------------------------|----------------------------|
| 2 (sin mutex) | entre 1,075,562 y 1,135,545, distinto en cada ejecución | No, en ninguna ejecución |
| 3 (con mutex) | 4,000,000 en todas | Sí, en las cinco ejecuciones |

En la Parte 3 el valor obtenido coincidió con el esperado en las cinco ejecuciones, y además fue siempre el mismo. Con el mutex ya no se pierden incrementos.

### 1. ¿Qué cambió con respecto al programa anterior?

Se agregó un `std::mutex` global llamado `mtx` y, dentro del ciclo de `incrementar`, un `std::lock_guard` que lo bloquea justo antes de `contador++`. El resto del programa es igual, con los mismos 4 hilos y el mismo millón de repeticiones por hilo. Ese único cambio hizo que el resultado pasara de unos 1 millón a los 4000000 exactos en las cinco ejecuciones.

### 2. ¿Qué hace `std::mutex`?

Es una herramienta de sincronización que sirve para proteger datos compartidos de accesos simultáneos de varios hilos [6]. Cuando un hilo logra bloquearlo con `lock`, pasa a ser su dueño y cualquier otro hilo que intente bloquearlo se queda esperando hasta que el dueño lo libere con `unlock` [6]. De esa forma solo un hilo a la vez puede ejecutar la parte del código que el mutex protege, que se conoce como sección crítica.

### 3. ¿Qué hace `std::lock_guard`?

Es un envoltorio del mutex que sigue la idea de RAII. Al crearse intenta tomar el mutex y cuando el programa sale del bloque donde fue creado se destruye y libera el mutex automáticamente [7]. En este programa eso significa que el mutex se bloquea al inicio de cada vuelta del ciclo y se libera al terminar esa vuelta. La ventaja es que no hay que acordarse de llamar a `unlock`, y el mutex también se libera si ocurre una excepción dentro del bloque [6].

### 4. ¿Por qué ahora el resultado sí debería ser correcto?

Porque la función del mutex hace que solo un hilo a la vez ejecute `contador++`. Los tres pasos de la operación, leer, sumar y escribir [5], ya no se pueden mezclar con los de otro hilo, puesto que el hilo que tiene el mutex, limita el acceso de los datos que tiene, a los demás, de manera que nunca hay dos hilos leyendo el mismo valor al mismo tiempo. Los accesos a `contador` quedan ordenados uno detrás de otro, y según la definición de carrera de datos [3] ya no se cumple la condición de que no haya orden entre los accesos. Los resultados de la tabla lo confirman, porque en las cinco ejecuciones el valor obtenido fue exactamente 4,000,000.

### 5. ¿Qué desventaja podría tener proteger cada incremento individual con un mutex?

La consecuecia de agregar un mutex, es que se pierde paralelismo y se agrega un costo en cada incremento. Como solo un hilo puede incrementar a la vez, los demás pasan el tiempo esperando, y además se hacen 4000000 bloqueos y desbloqueos, uno por cada incremento, lo que aumenta el tiempo de ejecución.

## Parte 4: mini benchmark secuencial vs. paralelo

El programa se compiló con `g++ -std=c++17 -O2 -Wall -Wextra -pthread src/parte4_benchmark.cpp -o parte4` (sin warnings) y se ejecutó `./parte4` cinco veces, más de las tres que pide el enunciado. Suma un vector de 100,000,000 de enteros con valor 1, primero con un solo hilo y luego dividido entre 4 hilos. La máquina tiene 20 núcleos lógicos según `nproc`.

### Resultados

| Ejecución | Tiempo secuencial | Tiempo paralelo | ¿Cuál fue más rápido? |
|-----------|-------------------|-----------------|------------------------|
| 1 | 37 ms | 11 ms | Paralelo |
| 2 | 37 ms | 11 ms | Paralelo |
| 3 | 32 ms | 14 ms | Paralelo |
| 4 | 28 ms | 9 ms | Paralelo |
| 5 | 29 ms | 10 ms | Paralelo |

En las cinco ejecuciones el resultado de la suma fue 100000000 tanto en la versión secuencial como en la paralela. La versión paralela fue la más rápida en todas.

### 1. ¿El resultado secuencial y el paralelo son iguales?

Sí. En las cinco ejecuciones las dos versiones dieron 100000000. Como el vector está lleno de unos, la suma tiene que ser igual a la cantidad de elementos, y las dos coinciden con ese valor. A diferencia de la Parte 2, aquí no hay condición de carrera, porque cada hilo suma su propio bloque y guarda el resultado en su propia posición del vector `resultados`.

### 2. ¿La versión paralela siempre fue más rápida?

En las cinco ejecuciones sí fue mucho más rápida, entre dos a tres veces más rápida. Por ejemplo 11 ms contra 37 ms en la ejecución 1 y 9 ms contra 28 ms en la ejecución 4.

### 3. ¿Por qué dividir el vector en bloques permite paralelizar el trabajo?

Porque la suma de cada bloque no depende de los demás. Cada hilo recibe un rango distinto del vector, lo recorre por su cuenta y deja su suma parcial en su propia posición de `resultados`. Como ningún hilo modifica una posición de memoria que otro esté usando, no hay conflicto entre ellos y no hace falta un mutex [3]. Al final el hilo principal solo suma las sumas parciales. Así los 4 hilos pueden trabajar al mismo tiempo en núcleos distintos, y cada uno hace aproximadamente una cuarta parte del trabajo.

### 4. ¿Qué costos adicionales tiene la versión paralela?

Tiene costos que la versión secuencial no tiene. Hay que crear los 4 hilos y esperarlos con `join()`, lo que le pide trabajo al sistema operativo [1]. También hay que reservar el vector `resultados`, repartir el vector en bloques, y al final sumar las sumas parciales en el hilo principal. Además, el planificador del sistema tiene que asignar los hilos a los núcleos. Todos esos costos son fijos, es decir, se pagan sin importar cuántos elementos haya que sumar. Si el trabajo de cada hilo es pequeño, esos costos pueden superar lo que se ahorra por repartirlo [8]. Por eso en las mediciones el paralelo no llegó a ser 4 veces más rápido con 4 hilos, aunque no se midió cuánto aportó cada costo por separado.

### 5. ¿Qué podría pasar si el vector fuera muy pequeño?

La versión paralela podría ser más lenta que la secuencial, porque el costo de crear y esperar los hilos sería mayor que el tiempo que se ahorra al repartir el trabajo [8]. Crear los hilos, esperarlos con `join()` y combinar los resultados parciales cuesta lo mismo sin importar el tamaño del vector, pero con pocos elementos cada hilo recibiría un bloque tan pequeño que terminaría casi de inmediato, y el tiempo total quedaría dominado por esos costos y no por la suma. En ese caso el resultado seguiría siendo correcto, solo que más lento que la versión secuencial.


## Parte 5: cantidad de hilos vs. rendimiento

El programa se compiló con `g++ -std=c++17 -O2 -Wall -Wextra -pthread src/parte5_hilos_vs_rendimiento.cpp -o parte5` (sin warnings) y se ejecutó `./parte5` tres veces. Cada ejecución suma el mismo vector de 100,000,000 de enteros con 1, 2, 4, 8 y 16 hilos, y en todas las pruebas el resultado fue 100000000.

### Resultados

| Cantidad de hilos | Tiempo obtenido |
|-------------------|-----------------|
| 1 | 30.3 ms (33, 29 y 29 ms) |
| 2 | 16.7 ms (18, 15 y 17 ms) |
| 4 | 10.3 ms (11, 10 y 10 ms) |
| 8 | 9.0 ms (8, 10 y 9 ms) |
| 16 | 7.7 ms (8, 8 y 7 ms) |

El tiempo de cada fila es el promedio de las tres ejecuciones, y entre paréntesis están los tiempos de cada una.

### 1. ¿Cuál cantidad de hilos produjo el mejor tiempo?

La de 16 hilos, con un promedio de 7.7 ms (8, 8 y 7 ms en las tres ejecuciones), seguida por la de 8 hilos con 9.0 ms. La diferencia entre las dos es de apenas un par de milisegundos, que está cerca de lo que varía una ejecución de otra, así que el mejor tiempo de 16 hilos no es tan contundente como el salto que hubo de 1 a 4 hilos.

### 2. ¿El tiempo mejoró siempre al aumentar los hilos?

No siempre. En promedio el tiempo bajó en cada paso (30.3, 16.7, 10.3, 9.0 y 7.7 ms), pero cada mejora fue más pequeña que la anterior. De 1 a 2 hilos bajó unos 13.7 ms, de 2 a 4 unos 6.3 ms, y de 4 a 8 y de 8 a 16 unos 1.3 ms cada vez. Además, en algunas ejecuciones individuales no hubo mejora al duplicar los hilos. En la ejecución 2, 4 y 8 hilos tardaron lo mismo, 10 ms, y en la ejecución 1, 8 y 16 hilos también empataron con 8 ms.

### 3. ¿Cuántos núcleos tiene la computadora donde se ejecutó el programa?

Según `nproc` y `lscpu`, la computadora tiene 10 núcleos físicos y 20 hilos lógicos, porque cada núcleo físico ejecuta 2 hilos a la vez.

| Dato | Valor |
|------|-------|
| `nproc` | 20 |
| Procesador (`lscpu`) | 13th Gen Intel(R) Core(TM) i7-13650HX |
| Núcleos físicos | 10 (1 socket) |
| Hilos por núcleo | 2 |
| CPU(s) lógicos | 20 |

### 4. ¿Qué ocurre cuando se usan más hilos que núcleos disponibles?

No todos los hilos pueden ejecutarse al mismo tiempo, así que el sistema operativo reparte el tiempo de la CPU entre ellos. Deja correr un rato a cada hilo y luego pasa a otro, de manera que los hilos que no tienen un núcleo libre esperan su turno [9]. Esto significa que agregar hilos ya no agrega trabajo en paralelo y, en cambio, suma el costo de cambiar de un hilo a otro [10]. En esta computadora los 16 hilos de la prueba no superan los 20 hilos lógicos, por lo que este caso no se llegó a medir y esta respuesta se apoya en la teoría y no en un dato propio. Lo que sí podría influir es que 16 hilos superan los 10 núcleos físicos, y dos hilos del mismo núcleo físico comparten parte de sus recursos, así que no rinden el doble que uno solo. Esto podría ayudar a explicar por qué la mejora se fue haciendo cada vez más pequeña, aunque no se midió.

### 5. ¿Qué relación tiene esto con el context switching?

El context switching es el mecanismo con el que el sistema operativo se turna los hilos en un mismo núcleo. Consiste en guardar los valores de los registros del hilo que se estaba ejecutando y restaurar los del siguiente hilo que le toca [9]. Ese cambio toma tiempo en el que no se avanza en la suma, y además puede perjudicar el rendimiento de forma indirecta porque los hilos comparten la memoria caché del procesador [10]. Cuantos más hilos haya por encima de los núcleos disponibles, más cambios de contexto se necesitan y más tiempo se pierde en ellos. En las mediciones de esta computadora no se vio este efecto porque nunca se superaron los 20 hilos lógicos.

### 6. ¿Por qué la versión con 16 hilos podría no ser la mejor?

En una computadora con menos de 16 hilos de hardware habría más hilos que núcleos, y aparecerían los cambios de contexto y las esperas descritas antes. Además, con más hilos hay que pagar más costos de crear y esperar hilos, y cada uno recibe un bloque más pequeño del vector, por lo que el costo fijo pesa más frente al trabajo útil [8]. En los datos de esta computadora 16 hilos sí fue el mejor promedio, pero solo por 1.3 ms respecto a 8 hilos, y la tendencia era que cada hilo adicional aportaba cada vez menos. Por eso no se puede asegurar que seguir subiendo la cantidad de hilos siga mejorando el tiempo, y probablemente dejaría de hacerlo al pasar de los 20 hilos lógicos.

## Parte 6: ejemplo simple de deadlock

El programa se compiló con `g++ -std=c++17 -Wall -Wextra -pthread src/parte6_deadlock.cpp -o parte6` (sin warnings).

### Resultados

En las tres ejecuciones el programa no terminó. Se terminó el programa con Ctrl + C. Nunca apareció el mensaje "obtuvo ambos recursos" de ninguno de los dos hilos. Esta es la salida de la ejecución 1.

```text
Hilo B intentando tomar recurso 2...
Hilo A intentando tomar recurso 1...
Hilo B intentando tomar recurso 1...
Hilo A intentando tomar recurso 2...
```

| Ejecución | Primer mensaje impreso | ¿Terminó? | 
|-----------|------------------------|-----------|
| 1 | Hilo B (recurso 2) | No |
| 2 | Hilo A (recurso 1) | No |
| 3 | Hilo B (recurso 2) | No |

El orden de los mensajes cambió entre ejecuciones, como en la Parte 1, pero el resultado fue siempre el mismo. 

### 1. ¿El programa termina normalmente?

No. En ninguna de las tres ejecuciones terminó. Se quedó detenido después de imprimir que cada hilo intentaba tomar su segundo recurso, y tuvo que ser detenido pmanualmente.

### 2. ¿Qué recurso tomó primero el hilo A?

El recurso 1. En `hilo_a`, lo primero que se hace es `recurso1.lock()`, y solo después de una pausa de 100 ms intenta tomar el recurso 2.

### 3. ¿Qué recurso tomó primero el hilo B?

El recurso 2. En `hilo_b` el orden es el contrario, porque primero hace `recurso2.lock()` y después de la pausa intenta tomar el recurso 1.

### 4. ¿Por qué ninguno de los dos hilos puede continuar?

Porque cada hilo tiene un recurso que el otro necesita y a la vez espera el que tiene el otro. El hilo A tiene el recurso 1 y pide el recurso 2, que está en manos del hilo B, y el hilo B tiene el recurso 2 y pide el recurso 1, que está en manos del hilo A. Cuando un hilo es dueño de un mutex, los demás que lo pidan se quedan bloqueados hasta que el dueño lo libere [6], pero ninguno de los dos puede liberar el suyo porque está bloqueado esperando el otro. Además, un mutex no se le puede quitar a la fuerza a un hilo que lo tiene [11]. La pausa de 100 ms en cada función hace que sea casi seguro que ambos hilos alcancen a tomar su primer recurso antes de pedir el segundo, y por eso se bloquea en todas las ejecuciones.

### 5. ¿Qué significa espera circular?

Es una situación en la que existe una cadena circular de hilos, y cada uno tiene recursos que el siguiente de la cadena está pidiendo [11]. En este programa la cadena es de solo dos hilos, el A espera lo que tiene el B y el B espera lo que tiene el A. Es una de las cuatro condiciones necesarias para que ocurra un deadlock, y si alguna de ellas no se cumple, el deadlock no puede darse [11].

### 6. ¿Cómo se podría evitar este problema?

La forma más directa es romper la espera circular haciendo que todos los hilos tomen los recursos siempre en el mismo orden [11]. Por ejemplo, si el hilo B también tomara primero el recurso 1 y después el recurso 2, uno de los dos hilos obtendría ambos y el otro esperaría a que los libere, sin quedar bloqueados entre sí. Otra opción es tomar los dos recursos en una sola operación, que es lo que se hace en la versión corregida con `std::scoped_lock`.

### Versión corregida con `std::scoped_lock`

La versión corregida se compiló con `g++ -std=c++17 -Wall -Wextra -pthread src/parte6_deadlock_corregido.cpp -o parte6_corregido` (sin warnings) y se ejecutó `./parte6_corregido` cinco veces. En las cinco ejecuciones el programa terminó normalmente y los dos hilos obtuvieron ambos recursos. Esta es la salida de la ejecución 1.

```text
Hilo B obtuvo ambos recursos de forma segura.
Hilo A obtuvo ambos recursos de forma segura.
```

| Ejecución | Primer mensaje impreso | ¿Terminó? |
|-----------|------------------------|-----------|
| 1 | Hilo B | Sí |
| 2 | Hilo B | Sí |
| 3 | Hilo B | Sí |
| 4 | Hilo B | Sí |
| 5 | Hilo B | Sí |

A diferencia de la versión anterior, aquí el programa no se quedó bloqueado en ninguna ejecución.

### Pregunta final, ¿por qué `std::scoped_lock` ayuda a evitar este deadlock?

Porque toma los dos mutex juntos en una sola operación, y no uno por uno como en la versión con deadlock. Cuando se le pasan varios mutex, `std::scoped_lock` usa un algoritmo de evitación de deadlock, igual que `std::lock` [12]. Según la documentación, `std::lock` bloquea los objetos con una serie de llamadas a `lock`, `try_lock` y `unlock` cuyo orden no está especificado, y si algo falla libera lo que ya había tomado [13]. Por eso un hilo ya no se queda esperando el segundo recurso mientras retiene el primero, que era lo que bloqueaba a los dos hilos en la versión anterior. La documentación no detalla el mecanismo exacto, así que no se puede afirmar con certeza cómo decide cada paso. Además, el `std::scoped_lock` libera los mutex automáticamente cuando el hilo sale de la función [12], por lo que no hay que llamar a `unlock` manualmente. En este código los dos hilos piden los mutex en el mismo orden, lo cual por sí solo ya evitaría la espera circular [11], pero con `std::scoped_lock` la protección contra el deadlock no depende de que el programador se acuerde de ese orden.


PREGUNTAS FINALES

1. ¿Cuál fue la diferencia más clara que observó entre ejecución secuencial y ejecución con 
hilos? 

La diferencia más clara es el tiempo de ejecución de un programa. Cuando se ejecuta de manera secuencial las instrucciones se ejecutan una
 tras otra, por lo que el resultado es siempre el mismo y predecible, pero 
 el tiempo total tiende a ser mayor. Cuando se trabaja de forma paralela, el trabajo se puede repartir, lo que reduce el tiempo, pero, como se pudo observar en el laboratorio, el orden de los mensajes en la parte 1 cambiaba entre ejecuciones, y en la parte 2 el contador dio valores incorrectos y distintos en cada corrida. Es decir, con hilos se puede ganar velocidad, pero hay que cuidar el acceso a los datos compartidos para no obtener resultados erróneos.

2. ¿Qué es una condición de carrera? 

Una condición de carrera es cuando dos o más hilos acceden a un mismo recurso compartido al mismo tiempo, al menos uno de ellos lo modifica y no hay ningún mecanismo de sincronización. En ese caso el resultado depende del orden en que el sistema operativo intercale la ejecución de los hilos, y ese orden no se puede predecir. Por eso el mismo programa puede dar resultados distintos en cada ejecución. Se soluciona haciendo que el acceso al recurso sea exclusivo (con un mutex) o que la operación sea atómica.

3. ¿Por qué contador++ puede fallar cuando muchos hilos lo ejecutan al mismo tiempo? 

Relacionado a lo anterior, puede fallar ya que contador parece una sola operación en C++, pero a nivel de CPU son tres pasos: leer el valor desde memoria, incrementarlo y escribir el resultado de vuelta. Si dos hilos leen el mismo valor antes de que alguno haya escrito, ambos incrementan ese valor viejo y ambos escriben el mismo resultado. Por ejemplo, si el contador vale 5 y dos hilos lo leen al mismo tiempo, los dos escriben 6 cuando el valor correcto sería 7. Se pierde un incremento. Con millones de incrementos y 4 hilos, esto pasa muchas veces, y por eso el valor obtenido fue menor al esperado de 4000000.

4. ¿Qué problema resuelve un mutex? 

Mutex lo que hace es resolver el problema del acceso simultáneo a un recurso compartido. Funciona como un candado, puesto que solo un hilo puede tenerlo a la vez, y mientras lo tiene, los demás hilos que lo pidan se quedan esperando hasta que sea liberado.

5. ¿Qué ventaja tiene std::lock_guard sobre llamar manualmente a lock() y unlock()?

`std::lock_guard` sigue el principio RAII: bloquea el mutex cuando se crea y lo libera automáticamente cuando sale de su alcance, sin que el programador tenga que hacer nada. Con `lock()` y `unlock()` manuales es fácil olvidar el `unlock()`, o que la función termine antes por un `return` o una excepción sin haberlo llamado, lo cual deja el mutex bloqueado para siempre y los demás hilos se quedan esperando, produciendo un posible deadlock. Con `lock_guard` el mutex se libera siempre, incluso si ocurre una excepción, así que el código es más seguro y más corto.

6. ¿Por qué más hilos no siempre significan mejor rendimiento?

Porque cada hilo tiene un costo. Crear un hilo, administrarlo y luego esperarlo con `join()` consume tiempo y memoria. Además, un procesador solo puede ejecutar en paralelo tantos hilos como núcleos tiene, esto porque, si hay más hilos que núcleos, el sistema operativo debe alternar entre ellos (context switching), y cada cambio de contexto gasta tiempo que no se usa en trabajo útil. También puede haber competencia por recursos compartidos. Por ejemplo, si los hilos usan un mutex, se pueden quedar esperándose entre sí, y en tareas como sumar un vector grande el límite puede ser el ancho de banda de la memoria y no el procesador. Finalmente, si el trabajo es pequeño, el costo de crear los hilos puede ser mayor que el tiempo que se ahorra.

7. ¿Qué es un deadlock?

Un deadlock es una situación en la que dos o más hilos quedan esperando indefinidamente porque cada uno tiene un recurso que el otro necesita, y ninguno lo libera. Ninguno puede avanzar y el programa se queda congelado. En la parte 6 el hilo A tomaba primero el recurso 1 y luego pedía el 2, mientras que el hilo B tomaba primero el 2 y luego pedía el 1, así que cada uno quedó esperando el recurso que tenía el otro.

8. ¿Qué buenas prácticas aplicaría al programar con hilos?

- Reducir al mínimo los datos compartidos entre hilos. Si cada hilo trabaja con sus propios datos, como en la suma por bloques de la parte 4, no hace falta sincronizar.
- Proteger siempre con un mutex los datos compartidos que se modifican, y usar `std::lock_guard` o `std::scoped_lock` en lugar de `lock()` y `unlock()` manuales.
- Mantener las secciones críticas lo más cortas posible, para que los demás hilos no esperen de más.
- Si se necesitan varios mutex, usar `std::scoped_lock` o siempre adquirirlos en el mismo orden, para evitar deadlocks.
- Esperar a todos los hilos con `join()` antes de terminar el programa.
- No crear más hilos de los que se justifican, y usar como referencia la cantidad de núcleos disponibles.
- Medir el rendimiento antes de asumir que paralelizar mejora algo, y probar el programa muchas veces, porque los errores de concurrencia no siempre aparecen en cada ejecución.

9. ¿En qué tipo de programas reales podría ser útil la programación concurrente?

Es útil en programas que tienen que atender varias tareas a la vez o que pasan mucho tiempo esperando. Por ejemplo, servidores web que atienden a muchos clientes al mismo tiempo, aplicaciones con interfaz gráfica que deben seguir respondiendo mientras hacen otra tarea en segundo plano, programas que descargan archivos o consultan bases de datos, y sistemas embebidos que leen sensores, se comunican y controlan actuadores a la vez. En estos casos lo importante es que el programa no se quede bloqueado esperando una tarea, aunque no necesariamente haya varios núcleos trabajando al mismo tiempo.

10. ¿En qué tipo de programas reales podría ser útil la programación paralela?

Es útil en programas que requieren mucho cálculo y cuyo trabajo se puede dividir en partes independientes que se ejecutan al mismo tiempo en distintos núcleos. Por ejemplo, simulaciones científicas y de ingeniería, procesamiento de imágenes y video, renderizado de gráficos, entrenamiento de modelos de aprendizaje automático, análisis de grandes cantidades de datos y procesamiento de señales. Es el mismo principio de la parte 4, donde se divide un vector grande en bloques y cada hilo suma el suyo, pero aplicado a problemas mucho más grandes.


## Referencias

[1] cppreference, "std::thread::join". https://en.cppreference.com/w/cpp/thread/thread/join

[2] cppreference, "std::thread::~thread". https://en.cppreference.com/w/cpp/thread/thread/~thread

[3] cppreference, "Multi-threaded executions and data races". https://en.cppreference.com/cpp/language/multithread

[4] cppreference, "std::atomic<T>::operator++,++(int),--,--(int)". https://en.cppreference.com/cpp/atomic/atomic/operator_arith

[5] University of Chicago, CMSC 14300 Systems Programming I, "Lecture 12 - Race Conditions, Mutexes and Locks, Synchronization". https://classes.cs.uchicago.edu/current/14300-1/lectures/lec12_notes/

[6] cppreference, "std::mutex". https://en.cppreference.com/cpp/thread/mutex

[7] cppreference, "std::lock_guard". https://en.cppreference.com/cpp/thread/lock_guard

[8] U. A. Acar, A. Charguéraud y M. Rainey, "Oracle-Guided Scheduling for Controlling Granularity in Implicitly Parallel Languages", Journal of Functional Programming, 2016. https://www.chargueraud.org/research/2016/granularity/granularity_jfp16.pdf

[9] R. H. Arpaci-Dusseau y A. C. Arpaci-Dusseau, "Mechanism: Limited Direct Execution", en Operating Systems: Three Easy Pieces. https://pages.cs.wisc.edu/~remzi/OSTEP/cpu-mechanisms.pdf

[10] Wikipedia, "Context switch". https://en.wikipedia.org/wiki/Context_switch

[11] R. H. Arpaci-Dusseau y A. C. Arpaci-Dusseau, "Common Concurrency Problems", en Operating Systems: Three Easy Pieces. https://pages.cs.wisc.edu/~remzi/OSTEP/threads-bugs.pdf

[12] cppreference, "std::scoped_lock". https://en.cppreference.com/cpp/thread/scoped_lock

[13] cppreference, "std::lock". https://en.cppreference.com/cpp/thread/lock
