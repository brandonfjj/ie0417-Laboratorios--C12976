# Parte 14: Limpieza del ambiente

## Qué se hizo

Se revisaron los contenedores, imágenes, volúmenes y redes existentes en Docker. Luego se ejecutaron comandos de limpieza para eliminar los contenedores detenidos y las imágenes pendientes de eliminación. También se intentó limpiar los volúmenes no utilizados y se revisó el espacio ocupado por Docker. Finalmente, se ejecutó `docker system prune` para comprobar si quedaban otros recursos sin utilizar.

## Comandos ejecutados

| Comando | Para qué sirve |
|---|---|
| `docker ps -a` | Lista todos los contenedores, incluyendo los detenidos. |
| `docker images` | Lista las imágenes disponibles en Docker y muestra información sobre su uso y espacio. |
| `docker volume ls` | Lista los volúmenes existentes. |
| `docker network ls` | Lista las redes existentes. |
| `docker container prune` | Elimina todos los contenedores detenidos. |
| `docker image prune` | Elimina las imágenes pendientes de eliminación o *dangling images* que no están asociadas a un contenedor. [1] |
| `docker volume prune` | Elimina los volúmenes locales no utilizados; por defecto, se enfoca en volúmenes anónimos. [2] |
| `docker system df` | Muestra cuánto espacio en disco está utilizando Docker. [3] |
| `docker system prune` | Realiza una limpieza general de contenedores detenidos, redes no utilizadas, imágenes *dangling* y caché de construcción no utilizada. [4] |

## Resultado obtenido

### Recursos existentes antes de la limpieza

```text
$ docker ps -a

CONTAINER ID   IMAGE         COMMAND    CREATED        STATUS                    PORTS     NAMES
97b9349ce975   ubuntu        "bash"     38 hours ago   Exited (0) 37 hours ago             confident_wu
46e1ce6e7048   hello-world   "/hello"   38 hours ago   Exited (0) 38 hours ago             musing_swirles
e5b7e9981df3   hello-world   "/hello"   44 hours ago   Exited (0) 44 hours ago             dazzling_knuth
```

Había tres contenedores detenidos: `confident_wu`, `musing_swirles` y `dazzling_knuth`. Todos tenían estado `Exited (0)`.

```text
$ docker images

                                                        i Info →   U  In Use
IMAGE                   ID             DISK USAGE   CONTENT SIZE   EXTRA
hello-world:latest      5e2309035332       25.9kB         9.49kB    U
laboratorio-flask:1.0   b1b6522d36f1        222MB         54.3MB
nginx:latest            f9ea18bfa4fa        243MB         66.7MB
python:3.11-slim        6f31d6e9ba2b        200MB         50.8MB
redis:latest            c94085d298b7        213MB         57.6MB
ubuntu:latest           f144425ff09b        162MB         45.6MB    U
```

Se encontraron seis imágenes. Las imágenes `hello-world:latest` y `ubuntu:latest` aparecieron marcadas como `U` en la columna `EXTRA`, mientras que las demás no presentaron esa marca.

```text
$ docker volume ls

DRIVER    VOLUME NAME
local     datos-lab
```

Había un volumen llamado `datos-lab`, creado en la parte dedicada a persistencia con volúmenes.

```text
$ docker network ls

NETWORK ID     NAME      DRIVER    SCOPE
f8b427a67d7f   bridge    bridge    local
cdf10855c874   host      host      local
147e693a1e36   none      null      local
```

Se encontraban las tres redes mostradas por Docker: `bridge`, `host` y `none`. No aparecieron redes personalizadas en este listado.

### Limpieza de contenedores detenidos

```text
$ docker container prune

WARNING! This will remove all stopped containers.
Are you sure you want to continue? [y/N] y
Deleted Containers:
97b9349ce975ecc3fe96259120c2577b82679f71650135615aa3ba6cfc693b80
46e1ce6e7048e0a3afee1e183868f84f1585fddd60258e10125e5b62b0b00a5b
e5b7e9981df37b8598b70957af55d2028d8d8918394eb3ba765a96a215f235b8

Total reclaimed space: 20.48kB
```

El comando eliminó los tres contenedores detenidos que aparecieron anteriormente. En total se recuperaron `20.48kB` de espacio.

### Limpieza de imágenes

```text
$ docker image prune

WARNING! This will remove all dangling images.
Are you sure you want to continue? [y/N] y
Deleted Images:
untagged: sha256:afcbaa56c8519d5dfea883035f76f176f2fab6ffbb61761aac90ee4229dc8e7f
deleted: sha256:afcbaa56c8519d5dfea883035f76f176f2fab6ffbb61761aac90ee4229dc8e7f
untagged: sha256:df709a0d1a68988a93424c3fb2d853d85dc1568840d19200f2e98d4f3e2dca93
deleted: sha256:df709a0d1a68988a93424c3fb2d853d85dc1568840d19200f2e98d4f3e2dca93
untagged: sha256:4442b4ac94f104a9b619ea236c5ba9b24f4ddb4ff3207db2fa9e037a0b47ec48
deleted: sha256:4442b4ac94f104a9b619ea236c5ba9b24f4ddb4ff3207db2fa9e037a0b47ec48
untagged: sha256:5e2916caacec8155d4f062f4a3f03ccfb0c8d2a5224cd8aeba595a08f521886f
deleted: sha256:5e2916caacec8155d4f062f4a3f03ccfb0c8d2a5224cd8aeba595a08f521886f
untagged: sha256:a612ae74ab510633df4beeeeb1cb00a0b8b976312737c7942f1966123c5fe1dc
deleted: sha256:a612ae74ab510633df4beeeeb1cb00a0b8b976312737c7942f1966123c5fe1dc

Total reclaimed space: 39.47kB
```

Se eliminaron cuatro imágenes *dangling*. El espacio recuperado fue de `39.47kB`. El comando `docker image prune`, sin la opción `-a`, se limita a este tipo de imágenes no etiquetadas y no elimina todas las imágenes que simplemente no estén siendo utilizadas [1].

### Limpieza de volúmenes

```text
$ docker volume prune

WARNING! This will remove anonymous local volumes not used by at least one container.
Are you sure you want to continue? [y/N] y
Total reclaimed space: 0B
```

No se recuperó espacio mediante este comando. El volumen `datos-lab` no fue eliminado. Esto es consistente con que se trata de un volumen con nombre y con el comportamiento predeterminado de `docker volume prune`, que elimina volúmenes anónimos no utilizados y deja los volúmenes con nombre salvo que se indique una opción adicional [2].

### Espacio utilizado por Docker

```text
$ docker system df

TYPE            TOTAL     ACTIVE    SIZE      RECLAIMABLE
Images          6         0         724.8MB   159MB (21%)
Containers      0         0         0B        0B
Local Volumes   1         0         33B       33B (100%)
Build Cache     0         0         0B        0B
```

Después de las limpiezas, Docker tenía seis imágenes con un tamaño total de `724.8MB`. No había contenedores activos ni contenedores registrados en este momento. El volumen local `datos-lab` ocupaba `33B` y aparecía como espacio recuperable. Tampoco había caché de construcción.

### Limpieza general

```text
$ docker system prune

WARNING! This will remove:
  - all stopped containers
  - all networks not used by at least one container
  - all dangling images
  - unused build cache

Are you sure you want to continue? [y/N] y
Total reclaimed space: 0B
```

La limpieza general no recuperó espacio adicional. Esto indica que, después de los comandos anteriores, no quedaban contenedores detenidos, redes no utilizadas, imágenes *dangling* ni caché de construcción que este comando pudiera eliminar. `docker system prune` agrupa varios tipos de limpieza, pero no elimina los volúmenes por defecto [4].

## Qué recursos quedaron creados

Al inicio de la limpieza se encontraron tres contenedores detenidos, seis imágenes, un volumen llamado `datos-lab` y las redes `bridge`, `host` y `none`.

Los tres contenedores fueron eliminados con `docker container prune`. También se eliminaron cuatro imágenes *dangling* con `docker image prune`. El volumen `datos-lab` permaneció porque `docker volume prune` no eliminó este volumen con nombre. Las redes que aparecieron en el listado inicial permanecieron.

Después de la limpieza, `docker system df` mostró seis imágenes, ningún contenedor, un volumen local y ninguna caché de construcción.

## Qué comandos de limpieza se ejecutaron

Se ejecutaron los siguientes comandos:

- `docker container prune`, que eliminó los tres contenedores detenidos.
- `docker image prune`, que eliminó cuatro imágenes *dangling*.
- `docker volume prune`, que no eliminó ningún volumen y recuperó `0B`.
- `docker system prune`, que no recuperó espacio adicional.

No se ejecutó `docker system prune -a`.

## Diferencia entre limpiar contenedores, imágenes y volúmenes

Los contenedores son instancias creadas a partir de imágenes. En esta práctica, `docker container prune` eliminó los contenedores que estaban detenidos [1].

Las imágenes contienen los elementos necesarios para crear contenedores. `docker image prune` elimina las imágenes *dangling* de forma predeterminada, mientras que la opción `-a` amplía la limpieza a imágenes no utilizadas [1].

Los volúmenes están destinados a conservar datos independientemente del ciclo de vida de los contenedores. Por esta razón Docker es más conservador al eliminarlos: `docker volume prune` elimina por defecto volúmenes anónimos no utilizados, y los volúmenes con nombre requieren mayor cuidado [2].

## Resultado de `docker system df`

El resultado obtenido fue:

```text
TYPE            TOTAL     ACTIVE    SIZE      RECLAIMABLE
Images          6         0         724.8MB   159MB (21%)
Containers      0         0         0B        0B
Local Volumes   1         0         33B       33B (100%)
Build Cache     0         0         0B        0B
```

Por lo tanto, después de la limpieza, las imágenes eran el recurso que más espacio ocupaba, con `724.8MB`. Docker reportó `159MB` como espacio recuperable en imágenes y `33B` en el volumen local [3].

## Observaciones

- **Contenedores eliminados:** `docker container prune` eliminó los tres contenedores detenidos: `confident_wu`, `musing_swirles` y `dazzling_knuth`.
- **Espacio recuperado de contenedores:** se recuperaron `20.48kB`.
- **Imágenes eliminadas:** `docker image prune` eliminó cuatro imágenes *dangling* y recuperó `39.47kB`.
- **Imágenes restantes:** después de la limpieza quedaron seis imágenes, con un tamaño total reportado de `724.8MB`.
- **Volumen conservado:** `datos-lab` permaneció después de `docker volume prune`, y `docker system df` lo reportó con un tamaño de `33B`.
- **Redes:** el listado inicial mostró únicamente `bridge`, `host` y `none`.
- **Limpieza general:** `docker system prune` no recuperó espacio adicional (`0B`).
- **No se utilizó `-a`:** no se ejecutó `docker image prune -a` ni `docker system prune -a`, por lo que las imágenes etiquetadas que no estaban asociadas a contenedores no fueron eliminadas mediante estos comandos.
- **Limpieza de terminal:** no se modificaron los valores de las salidas. Los prompts del sistema se representaron como `$` para mantener el formato utilizado en las demás partes.
- **Qué no se probó:** no se ejecutó `docker image prune -a`, no se ejecutó `docker system prune -a`, no se ejecutó `docker volume prune -a` y no se eliminó manualmente el volumen `datos-lab`. Tampoco se ejecutó `docker network prune` de forma independiente.

## Preguntas de reflexión

### 1. ¿Por qué Docker puede consumir mucho espacio en disco?

Docker puede acumular espacio debido a imágenes, contenedores detenidos, volúmenes, redes y cachés de construcción que permanecen en el sistema hasta que se eliminan explícitamente [1]. En este laboratorio se pudo observar que, después de limpiar los contenedores y las imágenes *dangling*, todavía había seis imágenes ocupando `724.8MB`.

### 2. ¿Qué diferencia hay entre eliminar un contenedor y eliminar una imagen?

Eliminar un contenedor elimina una instancia creada a partir de una imagen. La imagen, en cambio, es el recurso que sirve como base para crear nuevos contenedores. En el laboratorio, `docker container prune` eliminó los tres contenedores detenidos, pero las imágenes utilizadas no desaparecieron por eso. La limpieza de imágenes se hizo por separado con `docker image prune`.

### 3. ¿Por qué se debe tener cuidado al eliminar volúmenes?

Porque los volúmenes pueden contener datos que se quieren conservar aunque se elimine el contenedor. Docker no elimina automáticamente los volúmenes precisamente para evitar la pérdida de datos [1]. En el laboratorio se conservó `datos-lab`, que había sido utilizado para almacenar información persistente en una parte anterior.

### 4. ¿Qué buenas prácticas aplicaría para mantener limpio su ambiente local?

Aplicaría limpiezas periódicas de contenedores detenidos y revisaría el espacio utilizado con `docker system df`. También eliminaría imágenes que ya no necesite, pero evitaría usar comandos como `docker system prune -a` sin revisar primero qué recursos podrían eliminarse. Para los volúmenes tendría especial cuidado y verificaría primero que no contengan datos importantes.

## Reflexión personal

Con esta parte entendí mejor que Docker no elimina automáticamente todos los recursos que dejan de utilizarse, por lo que se pueden acumular imágenes, contenedores y otros datos. Me pareció útil comparar el estado antes y después de ejecutar los comandos de limpieza. También entendí por qué los volúmenes requieren más cuidado: aunque un contenedor se elimine, los datos de un volumen pueden seguir siendo necesarios. En mi caso, la limpieza recuperó poco espacio directamente, pero `docker system df` permitió ver que las imágenes seguían representando la mayor parte del espacio utilizado.

## Referencias

[1] Docker Inc., "Prune unused Docker objects," *Docker Docs*. [En línea]. Disponible en: https://docs.docker.com/engine/manage-resources/pruning/. [Accedido: 6-oct-2026].

[2] Docker Inc., "docker volume prune," *Docker Docs*. [En línea]. Disponible en: https://docs.docker.com/reference/cli/docker/volume/prune/. [Accedido: 6-oct-2026].

[3] Docker Inc., "docker system," *Docker Docs*. [En línea]. Disponible en: https://docs.docker.com/reference/cli/docker/system/. [Accedido: 6-oct-2026].

[4] Docker Inc., "docker system prune," *Docker Docs*. [En línea]. Disponible en: https://docs.docker.com/reference/cli/docker/system/prune/. [Accedido: 6-oct-2026].