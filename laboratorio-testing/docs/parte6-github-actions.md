# GitHub Actions

## Parte 10: integración continua con GitHub Actions

### Qué se hizo

Se creó un workflow de GitHub Actions que compila el proyecto y ejecuta las pruebas cada vez que hay cambios hacia `main`. Luego se subió la rama `Laboratorio-testing`, se abrió un pull request hacia `main` y se revisó la ejecución en la pestaña Actions.

### Archivo creado

`.github/workflows/testing.yml`, con el contenido del enunciado.

El enunciado ubica la carpeta `.github/workflows/` dentro de `laboratorio-testing/`, pero GitHub solo lee esa carpeta en la raíz del repositorio [1]. Como `laboratorio-testing/` es una subcarpeta del repositorio, el archivo se creó en la raíz. Por eso se conservaron los `cd laboratorio-testing` del workflow, que en este caso sí hacen falta.

### Comandos ejecutados

Se ejecutaron desde la raíz del repositorio.

```bash
git status
git add .github
git commit -m "Agrega workflow de GitHub Actions para compilar y ejecutar las pruebas"
git push
```

Se usó `git add .github` en lugar de `git add .` para no agregar las carpetas de otros laboratorios que aparecen sin seguimiento. El workflow solo escucha eventos de `main`, así que el push a `Laboratorio-testing` no lo ejecutó. Se abrió un pull request hacia `main` desde GitHub y eso sí lo disparó.

### Resultado

La ejecución terminó con estado **Success**. Los cinco pasos del workflow, más los pasos que agrega GitHub al inicio y al final, salieron con ✔.

![Workflow exitoso en GitHub Actions](Captura_parte_10.png)

En el paso `Run tests` se ven las 42 pruebas aprobadas.

![Salida del paso Run tests](Captura_parte_10_run_test.png)

GitHub también mostró dos avisos, que no son errores. Uno indica que `actions/checkout@v4` usa una versión de Node que se está retirando, y el otro que la etiqueta `ubuntu-latest` pasará a Ubuntu 26.

### Qué es integración continua

Es una práctica en la que se sube código con frecuencia a un repositorio compartido y cada cambio se compila y se prueba de forma automática [2]. Así los errores se detectan pronto y es más fácil encontrar qué cambio los causó.

### Qué hace el archivo `testing.yml`

Define un workflow llamado `C++ testing` con un solo trabajo, `build-and-test`. Ese trabajo se ejecuta en una máquina virtual con Ubuntu que GitHub proporciona [1]. Instala las herramientas, configura el proyecto con CMake, lo compila y ejecuta las pruebas.

### En qué eventos se ejecuta

En dos eventos [1]. Cuando se hace un `push` a la rama `main` y cuando se abre o se actualiza un `pull_request` cuyo destino es `main`. Los push a otras ramas no lo ejecutan.

### Qué pasos ejecuta

| Paso | Qué hace |
|---|---|
| Checkout repository | Descarga el código del repositorio con `actions/checkout@v4` |
| Install dependencies | Instala `build-essential`, `cmake` y `lcov` con `apt` |
| Configure CMake | Crea `laboratorio-testing/build` y ejecuta `cmake ..` |
| Build | Compila con `make` |
| Run tests | Ejecuta `./run_tests` |

### Qué pasaría si una prueba falla en GitHub Actions

`./run_tests` terminaría con un código de error y el paso `Run tests` saldría con ✘. Eso haría que el trabajo completo fallara, y el pull request mostraría el resultado como fallido [2]. En el registro del paso quedaría visible cuál prueba falló y qué valores comparó, igual que en la terminal local.

### Qué se aprendió

Que el workflow solo se ejecuta si está en el lugar correcto y se dispara con el evento correcto. En este caso fue necesario ponerlo en la raíz del repositorio y abrir un pull request, porque un push a la rama de trabajo no lo activaba.

## Preguntas de reflexión de la Parte 10

### 1. ¿Por qué es útil ejecutar pruebas automáticamente en GitHub?

Porque las pruebas se ejecutan en una máquina limpia y sin depender de lo que tenga instalado cada persona. Además nadie tiene que acordarse de correrlas, ya que se ejecutan solas con cada cambio [2].

### 2. ¿Qué problema resuelve la integración continua?

Evita que los errores se acumulen sin ser detectados. Al probar cada cambio apenas se sube, se sabe pronto si algo se rompió y es más fácil encontrar el cambio responsable [2].

### 3. ¿Por qué conviene ejecutar pruebas antes de integrar cambios?

Porque una vez que el código está en `main`, los demás lo usan como base. Si las pruebas corren antes del merge, un error se ve en el pull request y no llega a la rama principal [2]. Eso fue lo que se hizo en este laboratorio, donde el workflow corrió sobre el pull request antes de integrar nada.

### 4. ¿Qué información proporciona GitHub Actions cuando un workflow falla?

Muestra qué trabajo y qué paso fallaron, y el registro completo de cada paso. En el caso de las pruebas, el registro incluye la salida de Google Test, con la prueba que falló y los valores esperado y obtenido. También marca el resultado en el pull request [2].

### 5. ¿Cómo ayuda CI a mejorar la colaboración en equipo?

Da una verificación común que no depende de la máquina de nadie. Quien revisa un pull request puede ver si las pruebas pasaron sin tener que ejecutarlas, y quien lo escribió sabe pronto si afectó el trabajo de los demás [2].

## Referencias

[1] GitHub, "Understanding GitHub Actions", en GitHub Docs. https://docs.github.com/en/actions/about-github-actions/understanding-github-actions

[2] GitHub, "About continuous integration with GitHub Actions", en GitHub Docs. https://docs.github.com/en/actions/about-github-actions/about-continuous-integration-with-github-actions
