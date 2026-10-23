# Laboratorio: depuración con gdb, valgrind y sanitizers

Curso IE0417 Diseño de Software para Ingeniería, Escuela de Ingeniería Eléctrica, Universidad de Costa Rica.

**Estudiante:** Brandon Fuentes Jiménez, carné C12976. Modalidad individual.

**Entorno:** WSL2 con Ubuntu 22.04, g++ 11.4.0, gdb 12.1, valgrind 3.18.1.

## Contenido

| Parte | Tema | Documento | Código |
|---|---|---|---|
| 1 | Instalación y verificación del ambiente | [parte1-instalacion.md](parte1-instalacion.md) | [prueba.cpp](codigo/prueba.cpp) |
| 2 | Tipos de errores | [parte2-tipos-errores.md](parte2-tipos-errores.md) | [01_error_sintaxis.cpp](codigo/01_error_sintaxis.cpp), [02_error_logico.cpp](codigo/02_error_logico.cpp) |
| 3 | Introducción a gdb | [parte3-gdb-basico.md](parte3-gdb-basico.md) | [03_gdb_basico.cpp](codigo/03_gdb_basico.cpp) |
| 4 | `step`, `next` y `backtrace` | [parte4-gdb-funciones.md](parte4-gdb-funciones.md) | [04_gdb_funciones.cpp](codigo/04_gdb_funciones.cpp) |
| 5 | Análisis de memoria con valgrind | [parte5-valgrind.md](parte5-valgrind.md) | [05_memory_leak.cpp](codigo/05_memory_leak.cpp), [06_invalid_access.cpp](codigo/06_invalid_access.cpp) |
| 6 | AddressSanitizer | [parte6-asan.md](parte6-asan.md) | [07_use_after_free.cpp](codigo/07_use_after_free.cpp) |
| 7 | Hilos y condiciones de carrera | [parte7-hilos.md](parte7-hilos.md) | [08_race_condition.cpp](codigo/08_race_condition.cpp), [09_race_condition_corregido.cpp](codigo/09_race_condition_corregido.cpp) |
| 8 y 9 | Mini reto de depuración y reflexión final | [parte8-reto-y-reflexion.md](parte8-reto-y-reflexion.md) | [10_reto.cpp](codigo/10_reto.cpp) |

## Notas

- Los comandos se ejecutan desde la carpeta `laboratorio-depuracion/`, por ejemplo `g++ -g -o nombre codigo/archivo.cpp`.
- En cada documento se muestra el código original con el error, y los archivos `.cpp` quedaron en su versión corregida. La excepción es `08_race_condition.cpp`, que conserva la condición de carrera porque la versión corregida es `09_race_condition_corregido.cpp`.
- Los ejecutables compilados no se versionan.
- En este entorno, ThreadSanitizer necesitó ejecutarse con `setarch -R` (ver la Parte 7).
