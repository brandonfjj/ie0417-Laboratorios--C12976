# Parte 1: Verificación de instalación de Docker

## Versión de Docker instalada

Se verificó la versión de Docker instalada mediante el comando:

```bash
docker --version
```

Resultado obtenido:

```text
Docker version 29.1.3, build 29.1.3-0ubuntu3~22.04.2
```

Por lo tanto, la versión instalada de Docker es 29.1.3.

## Sistema operativo utilizado

El laboratorio se realizó utilizando **Ubuntu 22.04 LTS mediante WSL2 (Windows Subsystem for Linux 2)**.
## Resultado inicial de `docker info`

Se ejecutó el comando:

```bash
docker info
```

En el primer intento, el comando mostró la información del cliente de Docker, pero no pudo obtener la información del servidor debido a un problema de permisos para acceder al socket de Docker.

Resultado obtenido:

```text
Client:
 Version:    29.1.3
 Context:    default
 Debug Mode: false
 Plugins:
  trust: Manage trust on Docker images (Docker Inc.)
    Version:  29.1.3
    Path:     /usr/libexec/docker/cli-plugins/docker-trust

Server:
permission denied while trying to connect to the docker API at unix:///var/run/docker.sock
```

Este resultado indica que el cliente de Docker estaba instalado, pero que el usuario actual no tenía permisos suficientes para comunicarse con el servicio de Docker mediante el socket `/var/run/docker.sock`.

## Diagnóstico y corrección del error de permisos

Para identificar la causa del problema se realizaron las siguientes verificaciones.

### 1. Verificar que el daemon de Docker estuviera en ejecución

```bash
sudo service docker status
```

El resultado mostró que el servicio estaba activo:

```text
Active: active (running)
```

Esto indicó que el daemon de Docker sí estaba funcionando y que el problema no era que el servicio estuviera detenido.

### 2. Revisar los permisos del socket de Docker

```bash
ls -l /var/run/docker.sock
```

Resultado obtenido:

```text
srw-rw---- 1 root docker 0 Oct  4 15:42 /var/run/docker.sock
```

Estos permisos indican que solo el usuario `root` y los usuarios que pertenecen al grupo `docker` pueden acceder al socket.

### 3. Revisar los grupos del usuario actual

```bash
groups
```

Resultado obtenido:

```text
brandonfj adm dialout cdrom floppy sudo audio dip video plugdev netdev
```

El usuario `brandonfj` no pertenecía al grupo `docker`, lo cual explica el error `permission denied`.

### 4. Agregar el usuario al grupo `docker`

```bash
sudo usermod -aG docker $USER
```

Luego se verificó la membresía del grupo con:

```bash
getent group docker
```

## Resultado final de `docker info`

Después de la corrección, el comando se ejecutó correctamente y mostró tanto la información del cliente como la del servidor:

```text
Client:
 Version:    29.1.3
 Context:    default
 Debug Mode: false
 Plugins:
  trust: Manage trust on Docker images (Docker Inc.)
    Version:  29.1.3
    Path:     /usr/libexec/docker/cli-plugins/docker-trust

Server:
 Containers: 0
  Running: 0
  Paused: 0
  Stopped: 0
 Images: 0
 Server Version: 29.1.3
 Storage Driver: overlayfs
  driver-type: io.containerd.snapshotter.v1
 Logging Driver: json-file
 Cgroup Driver: systemd
 Cgroup Version: 2
 Plugins:
  Volume: local
  Network: bridge host ipvlan macvlan null overlay
  Log: awslogs fluentd gcplogs gelf journald json-file local splunk syslog
 CDI spec directories:
  /etc/cdi
  /var/run/cdi
 Swarm: inactive
 Runtimes: io.containerd.runc.v2 runc
 Default Runtime: runc
 Init Binary: docker-init
 containerd version:
 runc version:
 init version:
 Security Options:
  seccomp
   Profile: builtin
  cgroupns
 Kernel Version: 6.6.87.2-microsoft-standard-WSL2
 Operating System: Ubuntu 22.04.5 LTS
 OSType: linux
 Architecture: x86_64
 CPUs: 20
 Total Memory: 7.568GiB
 Name: Brandon-ROG
 ID: 1dbb8400-fb0e-49d7-804b-32200193f6cd
 Docker Root Dir: /var/lib/docker
 Debug Mode: false
 Experimental: false
 Insecure Registries:
  127.0.0.0/8
  ::1/128
 Live Restore Enabled: false
 Firewall Backend: iptables
```

Con este resultado se comprobó que el cliente ya podía comunicarse con el daemon de Docker. Además, la salida muestra que no hay contenedores ni imágenes creadas todavía (`Containers: 0`, `Images: 0`), lo cual es esperable en una instalación nueva.

## Comando `docker help`

También se ejecutó el comando:

```bash
docker help
```

Este comando mostró correctamente la ayuda de Docker, incluyendo la sintaxis general de uso y los principales comandos disponibles.

Entre los comandos mostrados se encuentran:

* `run`: crear y ejecutar un nuevo contenedor.
* `exec`: ejecutar un comando dentro de un contenedor en ejecución.
* `ps`: listar contenedores.
* `build`: construir una imagen a partir de un Dockerfile.
* `pull`: descargar una imagen desde un registro.
* `images`: listar las imágenes disponibles localmente.
* `info`: mostrar información del sistema Docker.
* `stop`: detener contenedores.
* `rm`: eliminar contenedores.

La ejecución correcta de `docker help` permitió comprobar que la interfaz de línea de comandos de Docker está disponible y funcionando.

## ¿Qué información muestra `docker info`?

El comando `docker info` muestra información general sobre la instalación y el entorno de Docker. Proporciona datos tanto del cliente como del servidor, por ejemplo la versión, el contexto utilizado, los complementos disponibles, el número de contenedores e imágenes, el controlador de almacenamiento, la versión del kernel y los recursos del sistema (CPUs y memoria).

En la primera ejecución solo se pudo obtener la información del cliente debido al problema de permisos sobre `/var/run/docker.sock`. Después de corregirlo, también se obtuvo la información del servidor.

## ¿Por qué es importante verificar la instalación antes de continuar?

Es importante verificar la instalación de Docker antes de realizar las demás actividades del laboratorio porque las siguientes partes dependen de que Docker pueda comunicarse correctamente con su servicio y ejecutar contenedores.

La verificación permite detectar problemas relacionados con la instalación, el servicio de Docker o los permisos del usuario antes de ejecutar comandos como:

```bash
docker run
docker pull
docker build
docker ps
```

En este caso, la verificación permitió detectar y corregir un problema de permisos antes de continuar, lo que evitó errores en las partes siguientes del laboratorio.

## Reflexión

### ¿Qué diferencia hay entre instalar Docker y tener Docker ejecutándose correctamente?

Tener Docker instalado significa que las herramientas necesarias, incluyendo el cliente de línea de comandos, están disponibles en el sistema. Sin embargo, esto no garantiza que el cliente pueda comunicarse con el servicio de Docker ni que sea posible ejecutar contenedores [1].

En esta práctica se observó esta diferencia: `docker --version` funcionó desde el inicio, pero `docker info` no pudo acceder al servidor porque el usuario no pertenecía al grupo `docker` [2]. Fue necesario agregar el usuario a ese grupo y reiniciar la sesión de WSL para que Docker quedara funcionando correctamente.

### ¿Qué información útil muestra el comando `docker info`?

La información de `docker info` permite conocer el estado y las características del entorno de Docker, incluyendo información del cliente y del servidor [3]. Esto resulta útil para verificar que Docker funciona correctamente y para identificar posibles problemas de configuración.

En este caso, el resultado inicial fue útil porque permitió identificar que el cliente estaba instalado pero no podía comunicarse con el servidor, y el resultado final confirmó que el problema había sido corregido.

### ¿Por qué Docker necesita un servicio o daemon ejecutándose en segundo plano?

Docker utiliza un servicio (el daemon `dockerd`) que se encarga de realizar las operaciones relacionadas con los contenedores, las imágenes, las redes y otros recursos de Docker. El cliente de línea de comandos se comunica con este servicio para solicitar dichas operaciones [1].

Por esta razón, tener únicamente el comando `docker` disponible no es suficiente. También es necesario que el servicio esté activo y que el usuario tenga permisos para comunicarse con él, como se comprobó al resolver el error de acceso al socket `/var/run/docker.sock` [2].

## Referencias

[1] Docker Inc., "Docker overview: Docker architecture," *Docker Docs*. [En línea]. Disponible en: https://docs.docker.com/get-started/docker-overview/#docker-architecture. [Accedido: 4-oct-2026].

[2] Docker Inc., "Linux post-installation steps for Docker Engine: Manage Docker as a non-root user," *Docker Docs*. [En línea]. Disponible en: https://docs.docker.com/engine/install/linux-postinstall/. [Accedido: 4-oct-2026].

[3] Docker Inc., "docker system info," *Docker Docs*. [En línea]. Disponible en: https://docs.docker.com/reference/cli/docker/system/info/. [Accedido: 4-oct-2026].