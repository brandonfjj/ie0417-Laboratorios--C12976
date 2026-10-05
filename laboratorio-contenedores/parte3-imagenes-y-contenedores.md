# Parte 3: Imágenes y contenedores

## Objetivo

Comprender la diferencia entre una imagen y un contenedor, descargando una imagen de Ubuntu y ejecutando un contenedor interactivo a partir de ella.

## Comandos ejecutados

| Comando | Para qué sirve |
|---|---|
| `docker pull ubuntu` | Descarga la imagen `ubuntu` desde un registro (por defecto, Docker Hub) hacia el equipo local. |
| `docker images` | Lista las imágenes disponibles localmente. |
| `docker run -it ubuntu bash` | Crea e inicia un contenedor a partir de la imagen `ubuntu` y abre una terminal interactiva con `bash`. |
| `ls`, `pwd`, `cat /etc/os-release` | Dentro del contenedor: listan el directorio raíz, muestran el directorio actual y muestran la distribución instalada. |
| `exit` | Cierra la terminal del contenedor. |
| `docker ps -a` | Lista todos los contenedores, incluidos los detenidos. |

## Resultado obtenido

Descarga y listado de imágenes:

```text
$ docker pull ubuntu
Using default tag: latest
latest: Pulling from library/ubuntu
Digest: sha256:f144425ff09be612d6d9ad965196e9cdc23dae1f42110a8a11a3e9a8198759f7
Status: Image is up to date for ubuntu:latest
docker.io/library/ubuntu:latest

$ docker images
IMAGE                ID             DISK USAGE   CONTENT SIZE   EXTRA
hello-world:latest   5e2309035332       25.9kB         9.49kB    U
ubuntu:latest        f144425ff09b        162MB         45.6MB
```

Contenedor interactivo:

```text
$ docker run -it ubuntu bash
root@97b9349ce975:/# ls
bin   dev  home  lib64  mnt  proc  run   srv  tmp  var
boot  etc  lib   media  opt  root  sbin  sys  usr
root@97b9349ce975:/# pwd
/
root@97b9349ce975:/# cat /etc/os-release
PRETTY_NAME="Ubuntu 26.04.1 LTS"
NAME="Ubuntu"
VERSION_ID="26.04"
VERSION="26.04.1 LTS (Resolute Raccoon)"
VERSION_CODENAME=resolute
ID=ubuntu
ID_LIKE=debian
HOME_URL="https://www.ubuntu.com/"
SUPPORT_URL="https://help.ubuntu.com/"
BUG_REPORT_URL="https://bugs.launchpad.net/ubuntu/"
PRIVACY_POLICY_URL="https://www.ubuntu.com/legal/terms-and-policies/privacy-policy"
UBUNTU_CODENAME=resolute
LOGO=ubuntu-logo
root@97b9349ce975:/# exit
exit
```

Listado de contenedores después de salir:

```text
$ docker ps -a
CONTAINER ID   IMAGE         COMMAND    CREATED          STATUS                      PORTS     NAMES
97b9349ce975   ubuntu        "bash"     51 seconds ago   Exited (0) 8 seconds ago              confident_wu
46e1ce6e7048   hello-world   "/hello"   25 minutes ago   Exited (0) 25 minutes ago             musing_swirles
e5b7e9981df3   hello-world   "/hello"   7 hours ago      Exited (0) 7 hours ago                dazzling_knuth
```

## Qué hace `docker pull`

`docker pull` descarga una imagen desde un registro hacia el equipo local. Como no se indicó una etiqueta (*tag*), Docker usó `latest` por defecto, como lo muestra el mensaje `Using default tag: latest`. En esta ejecución el resultado fue `Image is up to date`, lo que significa que la imagen ya estaba descargada y que la versión local coincidía con la del registro (mismo *digest*), por lo que no se descargó nada nuevo.

## Qué muestra `docker images`

Muestra las imágenes almacenadas localmente, con su nombre y etiqueta, su ID y su tamaño. En el resultado aparecen dos imágenes: `hello-world` (de la Parte 2) y `ubuntu`. La columna `DISK USAGE` indica el espacio que ocupa la imagen en disco (162 MB para `ubuntu`), mientras que `CONTENT SIZE` indica el tamaño del contenido de la imagen (45.6 MB). La marca `U` junto a `hello-world` indica que la imagen está siendo usada por algún contenedor.

## Qué significa ejecutar un contenedor en modo interactivo

Ejecutar un contenedor en modo interactivo significa que la terminal del usuario queda conectada al proceso principal del contenedor, de forma que se pueden escribir comandos y ver su salida en tiempo real. Esto se logra con dos opciones: `-i` mantiene abierta la entrada estándar y `-t` asigna una pseudo-terminal. En este caso, el proceso principal es `bash`, por lo que el prompt cambió a `root@97b9349ce975:/#`, indicando que los comandos se estaban ejecutando dentro del contenedor y no en WSL.

## Qué se observó dentro del contenedor Ubuntu

- `ls` mostró la estructura típica de directorios de un sistema Linux (`bin`, `etc`, `home`, `usr`, `var`, entre otros).
- `pwd` mostró que el directorio de trabajo inicial es la raíz `/`.
- `cat /etc/os-release` mostró que el contenedor tiene **Ubuntu 26.04.1 LTS**. Esto es distinto del sistema anfitrión, que según la Parte 1 usa Ubuntu 22.04.5 LTS. Es decir, el contenedor trae su propia distribución, independiente de la del host.
- El usuario era `root` y el nombre del equipo (`97b9349ce975`) coincide con el ID del contenedor.

## Qué ocurrió al salir del contenedor

Al ejecutar `exit` se terminó `bash`, que era el proceso principal del contenedor, y por lo tanto el contenedor se detuvo. En `docker ps -a` aparece con estado `Exited (0)`, donde el código `0` indica una finalización sin errores. Como no se usó `--name`, Docker le asignó el nombre aleatorio `confident_wu`. El contenedor no se eliminó: sigue registrado en el sistema, aunque ya no está en ejecución.

## Preguntas de reflexión

### 1. ¿La imagen Ubuntu es lo mismo que una máquina virtual Ubuntu?

No. Una máquina virtual emula hardware completo y ejecuta su propio sistema operativo con su propio kernel sobre un hipervisor. En cambio, los contenedores virtualizan el sistema operativo y no el hardware, y se ejecutan como procesos aislados que comparten el kernel de la máquina anfitriona [1]. Además, una imagen es solo una plantilla de archivos: no se ejecuta por sí sola, sino que hay que crear un contenedor a partir de ella [2].

### 2. ¿Por qué el contenedor puede parecer un sistema Linux si no es una máquina virtual completa?

Porque la imagen `ubuntu` incluye el sistema de archivos de la distribución (directorios, binarios, bibliotecas y archivos de configuración), y el contenedor trabaja sobre ese sistema de archivos [2]. Por eso, al entrar se ven los directorios habituales de Linux y `/etc/os-release` reporta Ubuntu. Lo único que el contenedor no incluye es un kernel propio, que es lo que sí tendría una máquina virtual.

### 3. ¿Qué significa que el contenedor comparta el kernel con el host?

Significa que todos los contenedores del equipo, junto con el host, usan un único kernel de Linux, y no cada uno el suyo [1]. El aislamiento entre contenedores no lo da un kernel separado, sino mecanismos del propio kernel: al ejecutar `docker run`, Docker crea un conjunto de *namespaces* (que aíslan lo que el proceso puede ver, como procesos y red) y de *control groups* (que limitan los recursos que puede usar) para el contenedor [3].

En este laboratorio, el kernel compartido es el del entorno WSL2: en la Parte 1, `docker info` reportó la versión de kernel `6.6.87.2-microsoft-standard-WSL2`. Esto también explica cómo un contenedor con Ubuntu 26.04 pudo ejecutarse sobre un host con Ubuntu 22.04: lo que cambia es el espacio de usuario que trae la imagen, no el kernel.

### 4. ¿Qué diferencia hay entre una imagen descargada y un contenedor creado?

Una imagen es una plantilla inmutable, es decir, una vez creada no se puede modificar, y está compuesta por capas de solo lectura [2]. Un contenedor es una instancia creada a partir de esa imagen, que puede estar en ejecución o detenida. Al crear un contenedor, Docker le agrega un directorio propio donde puede escribir cambios, sin alterar las capas originales de la imagen [4]. De esta forma, a partir de una misma imagen se pueden crear muchos contenedores independientes. En este laboratorio hay una sola imagen `ubuntu` y, a partir de ella, se crearon contenedores distintos (por ejemplo, `97b9349ce975` en esta parte).

## Reflexión personal

Con esta parte entendí que una imagen y un contenedor son cosas distintas: la imagen es la base que se descarga una sola vez, y los contenedores se pueden crear a partir de ella las veces que se necesite. También me llamó la atención que el contenedor reportara Ubuntu 26.04 mientras que mi sistema es Ubuntu 22.04, porque muestra que el contenedor trae su propio entorno aunque comparta el kernel con el host. Esto ayuda a entender por qué los contenedores son más livianos que una máquina virtual.


## Administración de contenedores

### Qué se hizo

Se creó un contenedor con nombre a partir de la imagen `ubuntu`, se creó un archivo dentro de él, y se comprobó si ese archivo se conservaba al salir, reiniciar el contenedor y volver a entrar. Finalmente se detuvo y se eliminó el contenedor.

### Comandos ejecutados

| Comando | Para qué sirve |
|---|---|
| `docker run -it --name mi-ubuntu ubuntu bash` | Crea e inicia un contenedor llamado `mi-ubuntu` y abre una terminal interactiva (`-it`) con `bash`. |
| `docker ps -a` | Lista todos los contenedores, incluidos los detenidos. |
| `docker start mi-ubuntu` | Inicia un contenedor que ya existe. |
| `docker exec -it mi-ubuntu bash` | Abre una nueva terminal dentro de un contenedor que está en ejecución. |
| `docker stop mi-ubuntu` | Detiene el contenedor en ejecución. |
| `docker rm mi-ubuntu` | Elimina el contenedor (debe estar detenido). |

### Resultado obtenido

Creación del contenedor y del archivo:

```text
$ docker run -it --name mi-ubuntu ubuntu bash
Unable to find image 'ubuntu:latest' locally
latest: Pulling from library/ubuntu
4e07a0f12b2c: Pull complete
06ad70e463aa: Pull complete
8f70d2bfe91a: Download complete
Digest: sha256:f144425ff09be612d6d9ad965196e9cdc23dae1f42110a8a11a3e9a8198759f7
Status: Downloaded newer image for ubuntu:latest
root@eb1b7ef02b2e:/# echo "Hola desde el contenedor" > mensaje.txt
root@eb1b7ef02b2e:/# cat mensaje.txt
Hola desde el contenedor
root@eb1b7ef02b2e:/# exit
exit
```

Verificación del contenedor después de salir:

```text
$ docker ps -a
CONTAINER ID   IMAGE         COMMAND    CREATED          STATUS                      PORTS     NAMES
eb1b7ef02b2e   ubuntu        "bash"     30 seconds ago   Exited (0) 6 seconds ago              mi-ubuntu
46e1ce6e7048   hello-world   "/hello"   19 minutes ago   Exited (0) 19 minutes ago             musing_swirles
e5b7e9981df3   hello-world   "/hello"   7 hours ago      Exited (0) 7 hours ago                dazzling_knuth
```

Reinicio del contenedor y verificación del archivo:

```text
$ docker start mi-ubuntu
mi-ubuntu
$ docker exec -it mi-ubuntu bash
root@eb1b7ef02b2e:/# cat mensaje.txt
Hola desde el contenedor
root@eb1b7ef02b2e:/# exit
exit
```

Detención, eliminación y verificación final:

```text
$ docker stop mi-ubuntu
mi-ubuntu
$ docker rm mi-ubuntu
mi-ubuntu
$ docker ps -a
CONTAINER ID   IMAGE         COMMAND    CREATED          STATUS                      PORTS     NAMES
46e1ce6e7048   hello-world   "/hello"   20 minutes ago   Exited (0) 20 minutes ago             musing_swirles
e5b7e9981df3   hello-world   "/hello"   7 hours ago      Exited (0) 7 hours ago                dazzling_knuth
```

### Uso de `--name`

La opción `--name` permite asignarle un nombre propio al contenedor, en este caso `mi-ubuntu`. Si no se usa, Docker asigna un nombre aleatorio, como ocurrió con `musing_swirles` y `dazzling_knuth` en la Parte 2. Con el nombre asignado se pudo referenciar el contenedor en los comandos posteriores (`start`, `exec`, `stop` y `rm`) sin necesidad de copiar su ID.

### Diferencia entre `docker start` y `docker run`

`docker run` crea un contenedor nuevo a partir de una imagen y lo inicia. Si la imagen no está disponible localmente, también la descarga, como se observó con `ubuntu:latest`. Por otro lado, `docker start` no crea nada nuevo: solo vuelve a iniciar un contenedor que ya existe y estaba detenido. Por eso, al ejecutar `docker start mi-ubuntu`, se reutilizó el mismo contenedor (mismo ID `eb1b7ef02b2e`) con todo su contenido.

### Uso de `docker exec`

`docker exec` ejecuta un comando dentro de un contenedor que ya está en ejecución. En este caso se usó `docker exec -it mi-ubuntu bash` para abrir una nueva terminal dentro de `mi-ubuntu` y revisar el archivo. A diferencia de `docker run`, no crea un contenedor nuevo, y solo funciona si el contenedor está en ejecución; por eso fue necesario ejecutar antes `docker start`. Además, al salir de esa terminal con `exit`, el contenedor sigue activo, porque el proceso principal no es la terminal abierta con `exec`.

### Diferencia entre detener y eliminar un contenedor

Detener un contenedor (`docker stop`) finaliza su ejecución, pero el contenedor sigue existiendo y aparece en `docker ps -a`, por lo que se puede volver a iniciar con `docker start`. Eliminarlo (`docker rm`) lo borra del sistema por completo; después de hacerlo, `mi-ubuntu` ya no aparece en `docker ps -a` y no se puede reiniciar.

### Qué pasó con el archivo creado dentro del contenedor

El archivo `mensaje.txt` se creó dentro del sistema de archivos del contenedor. Se conservó cuando el contenedor se detuvo y se volvió a iniciar, ya que `cat mensaje.txt` mostró el mismo contenido después de `docker start` y `docker exec`. Sin embargo, al eliminar el contenedor con `docker rm`, el archivo se elimina junto con él, porque forma parte de la capa de escritura propia de ese contenedor. Si se creara un contenedor nuevo con la imagen `ubuntu`, este no tendría el archivo, ya que la imagen original no fue modificada.

### Preguntas de reflexión

#### 1. ¿Qué ventaja tiene asignar nombres a los contenedores?

Los nombres facilitan identificar y administrar los contenedores, ya que son más fáciles de recordar y escribir que un ID como `eb1b7ef02b2e`. También ayudan a distinguir contenedores con distintos propósitos cuando hay varios en el sistema, y hacen que los comandos sean más claros y menos propensos a errores.

#### 2. ¿Qué diferencia hay entre crear un contenedor nuevo y reiniciar uno existente?

Crear un contenedor nuevo (`docker run`) genera un contenedor distinto a partir de una imagen, con un sistema de archivos limpio y un ID nuevo. Reiniciar uno existente (`docker start`) retoma el mismo contenedor con el estado que tenía cuando se detuvo, incluyendo los archivos que se hayan creado dentro. En este laboratorio, esto se comprobó porque `mensaje.txt` seguía existiendo después de `docker start`.

#### 3. ¿Qué sucede con los datos creados dentro de un contenedor si este se elimina?

Los datos se pierden, porque están almacenados en la capa de escritura del contenedor, que se elimina junto con él. Para conservar datos más allá de la vida de un contenedor se necesitan mecanismos externos, como volúmenes o bind mounts.

#### 4. ¿Por qué se dice que los contenedores son desechables?

Se dice que son desechables porque se pueden crear, detener y eliminar rápidamente, y porque se pueden volver a crear en cualquier momento a partir de la misma imagen, sin perder nada importante, siempre que los datos relevantes estén fuera del contenedor. Esto permite tratarlos como entornos temporales y reproducibles, en lugar de sistemas que deban mantenerse y repararse manualmente.

### Reflexión personal

En esta parte entendí mejor el ciclo de vida de un contenedor: se crea, se puede detener e iniciar varias veces, y finalmente se elimina. Me llamó la atención que el archivo se mantuviera después de reiniciar, pero que desapareciera al eliminar el contenedor, porque muestra que lo que se guarda dentro de un contenedor no es permanente. Esto me hace pensar que para guardar información importante hay que usar otros mecanismos y no depender del contenedor mismo.


## Referencias

[1] Docker Inc., "What is a container?," *Docker*. [En línea]. Disponible en: https://www.docker.com/what-container. [Accedido: 4-oct-2026].

[2] Docker Inc., "What is an image?," *Docker Docs*. [En línea]. Disponible en: https://docs.docker.com/get-started/docker-concepts/the-basics/what-is-an-image/. [Accedido: 4-oct-2026].

[3] Docker Inc., "Docker Engine security," *Docker Docs*. [En línea]. Disponible en: https://docs.docker.com/engine/security/. [Accedido: 4-oct-2026].

[4] Docker Inc., "Understanding the image layers," *Docker Docs*. [En línea]. Disponible en: https://docs.docker.com/get-started/docker-concepts/building-images/understanding-image-layers/. [Accedido: 4-oct-2026].

