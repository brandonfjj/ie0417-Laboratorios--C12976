# Laboratorio de pruebas de software con Google Test, cobertura y GitHub Actions

Curso IE0417 Diseño de Software para Ingeniería, Escuela de Ingeniería Eléctrica, Universidad de Costa Rica.

Estudiante: Brandon Fuentes Jiménez, carné C12976.

## Descripción

Proyecto en C++ con tres módulos pequeños (`calculator`, `string_utils` y `grade_utils`) que se prueban con Google Test. Se compila con CMake, se mide la cobertura con `gcov` y `lcov`, y las pruebas se ejecutan automáticamente con GitHub Actions.

## Estructura

```
laboratorio-testing/
├── README.md
├── CMakeLists.txt
├── include/   calculator.h, string_utils.h, grade_utils.h
├── src/       calculator.cpp, string_utils.cpp, grade_utils.cpp
├── tests/     test_calculator.cpp, test_string_utils.cpp, test_grade_utils.cpp
└── docs/      documentación de cada parte
```

El workflow de GitHub Actions está en `.github/workflows/testing.yml`, en la raíz del repositorio, porque GitHub solo lee esa carpeta desde ahí.

## Requisitos

Ubuntu 22.04 (probado en WSL2) con `g++`, `make`, `cmake` y `lcov`. Google Test se descarga solo al configurar el proyecto.

```bash
sudo apt install build-essential cmake lcov
```

## Compilar y ejecutar las pruebas

Desde `laboratorio-testing/`:

```bash
mkdir -p build
cd build
cmake ..
make
./run_tests
```

## Medir la cobertura

Desde `laboratorio-testing/`:

```bash
rm -rf build
mkdir build
cd build
cmake -DENABLE_COVERAGE=ON ..
make
./run_tests
lcov --capture --directory . --output-file coverage.info --rc lcov_branch_coverage=1
lcov --remove coverage.info '/usr/*' '*/_deps/*' '*/tests/*' --output-file coverage_filtered.info --rc lcov_branch_coverage=1
genhtml coverage_filtered.info --output-directory coverage_report --branch-coverage
```

El reporte queda en `build/coverage_report/index.html`.

## GitHub Actions

El workflow `C++ testing` compila el proyecto y ejecuta las pruebas con cada `push` a `main` y con cada pull request hacia `main`.

## Documentación

| Archivo | Partes del enunciado |
|---|---|
| [`docs/parte1-plan-pruebas.md`](docs/parte1-plan-pruebas.md) | 1 Preparación y 9 Pruebas funcionales |
| [`docs/parte2-pruebas-unitarias.md`](docs/parte2-pruebas-unitarias.md) | 2 Código inicial, 3 Primeras pruebas y 4 `EXPECT` vs `ASSERT` |
| [`docs/parte3-pruebas-fallidas.md`](docs/parte3-pruebas-fallidas.md) | 5 Pruebas fallidas |
| [`docs/parte4-casos-borde.md`](docs/parte4-casos-borde.md) | 6 Casos borde y 7 Semillas |
| [`docs/parte5-cobertura.md`](docs/parte5-cobertura.md) | 8 Cobertura |
| [`docs/parte6-github-actions.md`](docs/parte6-github-actions.md) | 10 GitHub Actions y 11 Fallo intencional en CI |
| [`docs/reflexion-final.md`](docs/reflexion-final.md) | 12 Mini reto y 13 Reflexión final |
