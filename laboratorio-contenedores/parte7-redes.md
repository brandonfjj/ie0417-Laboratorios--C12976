# Parte 12: Redes de Docker

## Qué se hizo

Se creó una red llamada `red-lab` y se conectaron a ella dos contenedores: `servidor-web` (Nginx) y `cliente` (Ubuntu). Desde `cliente` se instaló `curl` y se hizo una solicitud a `http://servidor-web`, usando el nombre del contenedor en lugar de una dirección IP. Al terminar se eliminaron los contenedores y la red.

## Comandos ejecutados

| Comando | Para qué sirve |
|---|---|
| `docker network create red-lab` | Crea una red llamada `red-lab`. |
| `docker network ls` | Lista las redes existentes. |
| `docker run -d --name servidor-web --network red-lab nginx` | Crea un contenedor Nginx en segundo plano y lo conecta a `red-lab`. |
| `docker run -it --name cliente --network red-lab ubuntu bash` | Crea un contenedor Ubuntu interactivo conectado a `red-lab`. |
| `apt update` / `apt install -y curl` | Dentro de `cliente`, actualizan la lista de paquetes e instalan `curl`. |
| `curl http://servidor-web` | Dentro de `cliente`, hace una solicitud HTTP al contenedor `servidor-web` usando su nombre. |
| `docker stop servidor-web` / `docker rm servidor-web` / `docker rm cliente` | Detienen y eliminan los contenedores. |
| `docker network rm red-lab` | Elimina la red. |

## Resultado obtenido

### Creación y listado de la red

```text
$ docker network create red-lab
7e1a6e6e53dd97d653364569603ffca29ffb83880294c00826bd574aed1c816a
$ docker network ls
NETWORK ID     NAME      DRIVER    SCOPE
f8b427a67d7f   bridge    bridge    local
cdf10855c874   host      host      local
147e693a1e36   none      null      local
7e1a6e6e53dd   red-lab   bridge    local
```

Además de las redes que Docker trae por defecto (`bridge`, `host` y `none`), aparece `red-lab`, creada con el driver `bridge` y alcance `local`.

### Contenedor `servidor-web`

```text
$ docker run -d --name servidor-web --network red-lab nginx
Unable to find image 'nginx:latest' locally
latest: Pulling from library/nginx
[...]
Status: Downloaded newer image for nginx:latest
2d6d9cbf56e7d2a5486c7c67dfe76e004f5300bffc78385f5a9c70066f101dc5
```

Como la imagen `nginx` no estaba en el equipo, Docker la descargó antes de crear el contenedor. Con `-d` el contenedor quedó en segundo plano y Docker mostró su ID.

### Contenedor `cliente`: instalación de `curl`

```text
$ docker run -it --name cliente --network red-lab ubuntu bash
root@ca49ae2910f4:/# apt update
[...]
Fetched 26.6 MB in 3s (8119 kB/s)
2 packages can be upgraded. Run 'apt list --upgradable' to see them.
root@ca49ae2910f4:/# apt install -y curl
[...]
Summary:
  Upgrading: 0, Installing: 30, Removing: 0, Not Upgrading: 2
  Download size: 6617 kB
  Space needed: 20.1 MB / 1004 GB available
[...]
Setting up curl (8.18.0-1ubuntu2.7) ...
Processing triggers for libc-bin (2.43-2ubuntu2.4) ...
```

La imagen de Ubuntu no incluye `curl`, por lo que se instaló junto con 29 dependencias más. Durante la instalación aparecieron mensajes de `debconf` sobre interfaces de diálogo no disponibles; son avisos y la instalación terminó correctamente (`Setting up curl`).

### Prueba de conexión con `curl`

```text
root@ca49ae2910f4:/# curl http://servidor-web
<!DOCTYPE html>
<html>
<head>
<title>Welcome to nginx!</title>
<style>
html { color-scheme: light dark; }
body { width: 35em; margin: 0 auto;
font-family: Tahoma, Verdana, Arial, sans-serif; }
</style>
</head>
<body>
<h1>Welcome to nginx!</h1>
<p>If you see this page, nginx is successfully installed and working.
Further configuration is required for the web server, reverse proxy,
API gateway, load balancer, content cache, or other features.</p>

<p>For online documentation and support please refer to
<a href="https://nginx.org/">nginx.org</a>.<br/>
To engage with the community please visit
<a href="https://community.nginx.org/">community.nginx.org</a>.<br/>
For enterprise grade support, professional services, additional
security features and capabilities please refer to
<a href="https://f5.com/nginx">f5.com/nginx</a>.</p>

<p><em>Thank you for using nginx.</em></p>
</body>
</html>
root@ca49ae2910f4:/# exit
exit
```

El contenedor `cliente` recibió la página de bienvenida de Nginx, lo que confirma que alcanzó a `servidor-web` a través de la red.

### Limpieza

```text
$ docker stop servidor-web
servidor-web
$ docker rm servidor-web
servidor-web
$ docker rm cliente
cliente
$ docker network rm red-lab
red-lab
```

## Qué es una red en Docker

Una red en Docker es el mecanismo que permite que los contenedores se comuniquen entre sí y con el exterior. Cada contenedor tiene su propio entorno de red aislado, y las redes son lo que conecta esos entornos [1]. Docker crea por defecto una red de tipo `bridge`, y también permite crear redes personalizadas [2]. En el laboratorio, `docker network ls` mostró las redes por defecto (`bridge`, `host`, `none`) y la red `red-lab` creada para la práctica.

## Qué hace `docker network create`

Crea una red nueva con el nombre indicado [3]. Si no se especifica un driver, se crea una red tipo `bridge`, como se vio en el listado, donde `red-lab` aparece con driver `bridge`. El comando devolvió el ID de la red (`7e1a6e6e53dd...`), que coincide con el `NETWORK ID` abreviado que mostró `docker network ls`.

## Qué significa conectar contenedores a la misma red

Significa que los contenedores pueden comunicarse directamente entre sí [2]. En el laboratorio, ambos contenedores se crearon con `--network red-lab`, por lo que `cliente` pudo hacer solicitudes a `servidor-web`. Los contenedores conectados a la misma red pueden comunicarse sin necesidad de publicar puertos hacia el host [6].

## Qué ocurrió al ejecutar `curl http://servidor-web`

`curl` hizo una solicitud HTTP desde `cliente` hacia `servidor-web` y recibió como respuesta la página "Welcome to nginx!". Esto muestra que Nginx estaba funcionando y que `cliente` pudo alcanzarlo por la red `red-lab`. No se indicó ningún puerto en la dirección, por lo que `curl` usó el puerto HTTP por defecto (80), que es donde Nginx escucha por defecto. Tampoco se publicó ningún puerto con `-p` al crear los contenedores.

## Por qué se pudo usar el nombre `servidor-web`

Porque en las redes personalizadas Docker incluye una resolución de nombres automática: el nombre del contenedor se traduce a su dirección IP dentro de la red [2]. Así, `cliente` pudo escribir `servidor-web` en lugar de buscar la IP del contenedor. Según la documentación, esta resolución automática por nombre es una diferencia entre las redes `bridge` personalizadas y la red `bridge` por defecto [2].

## Observaciones

- **No se publicó ningún puerto:** ninguno de los dos contenedores usó `-p`, y aun así `cliente` llegó a `servidor-web`. La comunicación dentro de una red no requiere publicar puertos; eso solo hace falta para acceder desde el host [6].
- **Descarga de la imagen:** `nginx` no estaba en el equipo, por lo que Docker la descargó al ejecutar el primer contenedor. Esto no ocurriría en una segunda ejecución.
- **`curl` no venía instalado:** la imagen de Ubuntu es mínima, por eso hubo que ejecutar `apt update` y `apt install -y curl` dentro de `cliente` antes de hacer la prueba. Esta instalación solo existe dentro de ese contenedor.
- **`cliente` ya estaba detenido:** al ejecutar `exit` en `bash`, el contenedor terminó, por lo que se pudo eliminar con `docker rm` sin `docker stop`. `servidor-web` seguía en segundo plano, así que sí se detuvo primero.
- **Salida de `apt` recortada:** las salidas de `apt update` y `apt install` son muy largas, por lo que en la evidencia de arriba se muestran solo las líneas principales (marcadas con `[...]`).
- **Qué no se probó:** no se hizo la misma prueba con la red `bridge` por defecto para comparar, no se usó la dirección IP de `servidor-web` en lugar del nombre, no se usó `docker network inspect` para ver los contenedores conectados, no se intentó acceder a Nginx desde el navegador del host y no se tomaron capturas de pantalla.

## Preguntas de reflexión

### 1. ¿Por qué los contenedores necesitan redes?

Porque cada contenedor tiene su propio entorno de red aislado [1]. Sin una red que los conecte, un contenedor no podría comunicarse con otros contenedores ni con el exterior. En el laboratorio, la red `red-lab` fue lo que permitió que `cliente` llegara al servidor de `servidor-web`.

### 2. ¿Qué ventaja tiene usar nombres de contenedor en lugar de direcciones IP?

Las direcciones IP las asigna Docker y pueden cambiar cuando un contenedor se elimina y se vuelve a crear, mientras que el nombre se mantiene. Además, el nombre es más fácil de recordar y de escribir en la configuración de una aplicación. En el laboratorio no hubo que averiguar la IP de `servidor-web`: bastó con usar su nombre, porque Docker lo resuelve automáticamente en la red personalizada [2].

### 3. ¿Qué diferencia hay entre publicar un puerto hacia el host y comunicarse dentro de una red Docker?

Publicar un puerto (`-p`) conecta un puerto del host con un puerto del contenedor, y sirve para acceder al contenedor desde fuera de Docker, por ejemplo desde el navegador del host [6]. Comunicarse dentro de una red Docker es una conexión entre contenedores que están en la misma red, y no requiere publicar puertos [6]. En la Parte 7 se usó `-p` para abrir la aplicación en el navegador; en esta parte no se usó `-p` y `cliente` igualmente pudo conectarse a `servidor-web`.

### 4. ¿Qué ejemplos reales podrían usar una red Docker?

- **Aplicación web y base de datos:** un contenedor con la aplicación (por ejemplo, la de Flask) que se conecta a otro contenedor con la base de datos usando su nombre.
- **Servidor web y aplicación:** un contenedor de Nginx que reenvía las solicitudes a un contenedor con la aplicación.
- **Aplicación y caché:** una aplicación que consulta un contenedor de caché, como Redis.
- **Varios servicios que trabajan juntos:** por ejemplo, una arquitectura de microservicios donde cada servicio corre en su propio contenedor.

## Reflexión personal

Con esta parte entendí que los contenedores están aislados por defecto y que una red es lo que les permite hablar entre sí. Me pareció muy práctico poder usar `servidor-web` como dirección, sin buscar ninguna IP, y también notar que no hizo falta publicar ningún puerto para que `cliente` llegara a Nginx. Esto me deja más clara la diferencia con la Parte 7: publicar puertos sirve para entrar desde el host, y la red sirve para que los contenedores se conecten entre ellos. Lo veo útil para aplicaciones con varias piezas, como una aplicación y su base de datos.


## Comunicacion entre servicios

### Qué se hizo

Se creó una red llamada `red-app` y se ejecutó en ella un contenedor con Redis (`redis-lab`), que hace de servicio de base de datos simulada. Luego se ejecutó un contenedor temporal (`cliente-redis`) en la misma red con la herramienta `redis-cli`, se conectó a `redis-lab` por su nombre y se probaron los comandos `ping`, `set` y `get`. Al terminar se eliminaron los contenedores y la red.

### Comandos ejecutados

| Comando | Para qué sirve |
|---|---|
| `docker network create red-app` | Crea la red `red-app` [3]. |
| `docker run -d --name redis-lab --network red-app redis` | Crea un contenedor Redis en segundo plano y lo conecta a `red-app` [5]. |
| `docker ps` | Lista los contenedores en ejecución para verificar que `redis-lab` está activo. |
| `docker run -it --name cliente-redis --network red-app redis redis-cli -h redis-lab` | Crea un contenedor temporal, interactivo y en la misma red, que ejecuta `redis-cli` apuntando al host `redis-lab` [5]. |
| `ping` / `set curso IE0417` / `get curso` | Dentro de `redis-cli`, prueban la conexión y guardan y leen un valor. |
| `docker stop redis-lab` / `docker rm redis-lab` / `docker rm cliente-redis` | Detienen y eliminan los contenedores. |
| `docker network rm red-app` | Elimina la red [4]. |

### Resultado obtenido

#### Creación de la red y del contenedor Redis

```text
$ docker network create red-app
f10b83461a64bbf9ca9cfc707a86d2a1318859f5af90a6e169a9e288878bcfe6
$ docker run -d --name redis-lab --network red-app redis
Unable to find image 'redis:latest' locally
latest: Pulling from library/redis
[...]
Status: Downloaded newer image for redis:latest
c3f21aedc0d81f38841c2a317833c004c7dcefdb17f967930f7165493e69f490
```

Como la imagen `redis` no estaba en el equipo, Docker la descargó antes de crear el contenedor.

#### Verificación con `docker ps`

```text
$ docker ps
CONTAINER ID   IMAGE     COMMAND                  CREATED          STATUS          PORTS      NAMES
c3f21aedc0d8   redis     "docker-entrypoint.s…"   13 seconds ago   Up 12 seconds   6379/tcp   redis-lab
```

`redis-lab` aparece en ejecución. En la columna `PORTS` solo se muestra `6379/tcp`, sin asignación a un puerto del host, porque no se usó `-p`.

#### Cliente Redis

```text
$ docker run -it --name cliente-redis --network red-app redis redis-cli -h redis-lab
redis-lab:6379> ping
PONG
redis-lab:6379> set curso IE0417
OK
redis-lab:6379> get curso
"IE0417"
redis-lab:6379> exit
```

El cliente se conectó a `redis-lab` en el puerto 6379 (como indica el prompt), respondió `PONG` al `ping`, confirmó con `OK` que guardó el valor y devolvió `"IE0417"` al consultarlo.

#### Limpieza

```text
$ docker stop redis-lab
redis-lab
$ docker rm redis-lab
redis-lab
$ docker rm cliente-redis
cliente-redis
$ docker network rm red-app
red-app
```

### Qué es Redis en este ejemplo

Redis es un almacén de datos en memoria de tipo clave-valor que se usa, entre otras cosas, como base de datos o caché [7]. En este ejemplo se usó como una base de datos simulada: un servicio con el que otro contenedor se comunica por la red para guardar y consultar datos, como se hizo con la clave `curso`.

### Qué representa `redis-lab`

Representa el servicio al que se conecta una aplicación, es decir, el contenedor que hace el papel de servidor de base de datos. Su nombre es lo que usó el cliente para encontrarlo en la red, de la misma forma que una aplicación real usaría el nombre del contenedor de su base de datos en su configuración.

### Cómo se conectó el cliente al servidor

Ambos contenedores se crearon con `--network red-app`, por lo que quedaron en la misma red [1]. El cliente ejecutó `redis-cli -h redis-lab`, donde `-h` indica el nombre del servidor al que conectarse [8]. Docker resolvió el nombre `redis-lab` a la dirección del contenedor, igual que ocurrió con `servidor-web` en la Parte 12 [2], y `redis-cli` usó el puerto 6379, que es el puerto por defecto de Redis [8]. No fue necesario publicar puertos ni conocer la dirección IP del servidor.

### Qué significa recibir `PONG`

`PONG` es la respuesta del servidor al comando `PING` [9]. Que el cliente lo haya recibido significa que la conexión con `redis-lab` funciona y que el servidor está activo y respondiendo. Después, los comandos `set` y `get` confirmaron que el cliente no solo se conectó, sino que pudo guardar y leer datos en el servicio.

### Qué enseñanza deja este ejemplo sobre aplicaciones con varios contenedores

Una aplicación puede dividirse en varios servicios, cada uno en su propio contenedor, que se conectan mediante una red y se encuentran por nombre. El cliente y el servidor son contenedores distintos, creados desde la misma imagen (`redis`), pero con roles diferentes: uno ejecuta el servidor y el otro ejecuta `redis-cli`. Con una aplicación web y su base de datos ocurriría lo mismo: la aplicación usaría el nombre del contenedor de la base de datos para comunicarse con ella.

### Observaciones

- **Imagen descargada:** `redis` no estaba en el equipo, por lo que Docker la descargó al ejecutar el primer contenedor.
- **Sin puertos publicados:** `docker ps` mostró solo `6379/tcp`, sin asignación al host, y aun así el cliente se conectó, porque la comunicación dentro de la red no requiere `-p`.
- **Una imagen, dos roles:** `redis-lab` ejecutó el comando por defecto de la imagen, mientras que `cliente-redis` ejecutó `redis-cli` con la misma imagen.
- **`cliente-redis` ya estaba detenido:** al salir de `redis-cli`, el contenedor terminó, por lo que se pudo eliminar con `docker rm` sin `docker stop`.
- **Terminal limpiada:** en la salida original apareció un carácter de control (`^[[3~`) al inicio de una línea, producto de una tecla presionada por error. Lo quité de la evidencia de arriba. También recorté la lista de capas descargadas de la imagen con `[...]`.
- **Qué no se probó:** no se comprobó si el valor `curso` se conserva al recrear el contenedor, no se probó la conexión desde el host, no se probó el cliente en otra red para ver que no se conecta, no se usó `docker network inspect` y no se tomaron capturas de pantalla.

### Preguntas de reflexión

#### 1. ¿Por qué una aplicación web podría necesitar comunicarse con una base de datos?

Porque la aplicación necesita guardar y consultar información que debe conservarse y compartirse, como usuarios, productos o pedidos. En lugar de guardar esos datos ella misma, los delega a un servicio especializado, que en este ejemplo se representó con Redis y la clave `curso`.

#### 2. ¿Por qué ambos contenedores deben estar en la misma red?

Porque cada contenedor tiene su propio entorno de red aislado, y es la red la que permite que se comuniquen [1]. Además, el nombre `redis-lab` solo lo resuelve Docker dentro de la red personalizada [2]. En el laboratorio, ambos contenedores se crearon con `--network red-app`, y eso permitió que el cliente encontrara al servidor por su nombre.

#### 3. ¿Qué ventaja tiene separar servicios en contenedores distintos?

Cada servicio tiene su propia imagen y se puede actualizar, reemplazar o reiniciar sin tocar los demás. También permite reutilizar imágenes oficiales ya hechas: en esta práctica se usó Redis sin instalar nada en el equipo, solo descargando la imagen `redis`.

#### 4. ¿Qué limitación tiene hacerlo manualmente con varios comandos `docker run`?

Hay que escribir y ejecutar cada comando a mano, en el orden correcto y con los nombres y la red bien puestos, y repetirlo todo cada vez. Es fácil equivocarse: en el laboratorio, repetir un comando causó un error por nombre duplicado. La limpieza también requiere varios comandos, y el procedimiento no queda guardado en un archivo que otra persona pueda reproducir. Una herramienta como Docker Compose permite describir todos los servicios y la red en un solo archivo.

### Reflexión personal

Con esta parte entendí mejor cómo se organizaría una aplicación real con varios contenedores: cada servicio va en su propio contenedor y se encuentran por nombre dentro de una red. Me pareció útil ver que el cliente solo necesitó escribir `redis-lab` para conectarse, igual que con `servidor-web` en la Parte 12. También noté que, aunque con pocos contenedores es manejable, hacer todo con comandos `docker run` separados ya es incómodo y fácil de equivocar

## Referencias

[1] Docker Inc., "Networking overview," *Docker Docs*. [En línea]. Disponible en: https://docs.docker.com/engine/network/. [Accedido: 6-oct-2026].

[2] Docker Inc., "Bridge network driver," *Docker Docs*. [En línea]. Disponible en: https://docs.docker.com/engine/network/drivers/bridge/. [Accedido: 6-oct-2026].

[3] Docker Inc., "docker network create," *Docker Docs*. [En línea]. Disponible en: https://docs.docker.com/reference/cli/docker/network/create/. [Accedido: 6-oct-2026].

[4] Docker Inc., "docker network rm," *Docker Docs*. [En línea]. Disponible en: https://docs.docker.com/reference/cli/docker/network/rm/. [Accedido: 6-oct-2026].

[5] Docker Inc., "docker container run," *Docker Docs*. [En línea]. Disponible en: https://docs.docker.com/reference/cli/docker/container/run/. [Accedido: 6-oct-2026].

[6] Docker Inc., "Port publishing and mapping," *Docker Docs*. [En línea]. Disponible en: https://docs.docker.com/engine/network/port-publishing/. [Accedido: 6-oct-2026].

[7] Docker Inc., "Redis - Official Image," *Docker Hub*. [En línea]. Disponible en: https://hub.docker.com/_/redis. [Accedido: 6-oct-2026].

[8] Redis Ltd., "Redis CLI," *Redis Docs*. [En línea]. Disponible en: https://redis.io/docs/latest/develop/tools/cli/. [Accedido: 6-oct-2026].

[9] Redis Ltd., "PING," *Redis Docs*. [En línea]. Disponible en: https://redis.io/docs/latest/commands/ping/. [Accedido: 6-oct-2026].