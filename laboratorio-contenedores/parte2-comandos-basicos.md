# Parte 2: Primer contenedor

## Objetivo

Ejecutar el primer contenedor utilizando una imagen existente en Docker Hub y observar su comportamiento mediante los comandos `docker run`, `docker ps` y `docker ps -a`.

---

## 1. Ejecución de `docker run hello-world`

### Qué se hizo

Se ejecutó un contenedor a partir de la imagen `hello-world`, que es una imagen de prueba oficial de Docker.

### Comando ejecutado

```bash
docker run hello-world
```

### Explicación

El comando `docker run` crea un contenedor nuevo a partir de una imagen y lo ejecuta. Al ejecutar `docker run hello-world`, ocurre lo siguiente:

1. El cliente de Docker contacta al daemon de Docker.
2. El daemon busca la imagen `hello-world` de forma local. Si no la encuentra, la descarga desde Docker Hub.
3. El daemon crea un contenedor nuevo a partir de esa imagen.
4. El contenedor ejecuta el programa `/hello`, que imprime un mensaje en la terminal.
5. El daemon envía esa salida al cliente, que la muestra en la terminal.

### Resultado obtenido

```text
Hello from Docker!
This message shows that your installation appears to be working correctly.

To generate this message, Docker took the following steps:
 1. The Docker client contacted the Docker daemon.
 2. The Docker daemon pulled the "hello-world" image from the Docker Hub.
    (amd64)
 3. The Docker daemon created a new container from that image which runs the
    executable that produces the output you are currently reading.
 4. The Docker daemon streamed that output to the Docker client, which sent it
    to your terminal.

To try something more ambitious, you can run an Ubuntu container with:
 $ docker run -it ubuntu bash

Share images, automate workflows, and more with a free Docker ID:
 https://hub.docker.com/

For more examples and ideas, visit:
 https://docs.docker.com/get-started/
```

### ¿Qué ocurrió si la imagen no estaba descargada?

Si la imagen `hello-world` no existe en el equipo, Docker la descarga automáticamente desde Docker Hub antes de crear el contenedor. En ese caso, la terminal muestra mensajes como `Unable to find image 'hello-world:latest' locally` y `Pulling from library/hello-world`, seguidos del progreso de la descarga.

En esta ejecución no aparecieron esos mensajes de descarga, y la salida de `docker ps -a` muestra que ya existía un contenedor creado hace varias horas con la misma imagen. Esto indica que la imagen ya estaba disponible localmente y Docker la reutilizó sin descargarla de nuevo. El texto "pulled the hello-world image from the Docker Hub" que aparece en el mensaje es una descripción general de los pasos que Docker realiza, y siempre se imprime aunque la imagen ya exista en el equipo.

### Reflexión

Este comando es útil porque permite comprobar, con una sola instrucción, que el cliente, el daemon y el acceso a Docker Hub funcionan de forma integrada. También muestra que Docker puede obtener una imagen y ejecutarla sin que el usuario tenga que instalar manualmente ningún programa.

---

## 2. Ejecución de `docker ps`

### Qué se hizo

Se listaron los contenedores que están actualmente en ejecución.

### Comando ejecutado

```bash
docker ps
```

### Explicación

Este comando muestra únicamente los contenedores que se encuentran en estado de ejecución en el momento en que se ejecuta.

### Resultado obtenido

```text
CONTAINER ID   IMAGE     COMMAND   CREATED   STATUS    PORTS     NAMES
```

La lista apareció vacía, es decir, no había ningún contenedor en ejecución.

### Reflexión

Este resultado fue consistente con lo esperado, ya que el contenedor de `hello-world` solo imprime un mensaje y termina de inmediato. Por eso ya no aparece como contenedor activo.

---

## 3. Ejecución de `docker ps -a`

### Qué se hizo

Se listaron todos los contenedores del sistema, incluyendo los que ya terminaron su ejecución.

### Comando ejecutado

```bash
docker ps -a
```

### Explicación

La opción `-a` (`--all`) hace que Docker muestre todos los contenedores, sin importar su estado: en ejecución, detenidos o finalizados.

### Resultado obtenido

```text
CONTAINER ID   IMAGE         COMMAND    CREATED          STATUS                      PORTS     NAMES
46e1ce6e7048   hello-world   "/hello"   16 seconds ago   Exited (0) 15 seconds ago             musing_swirles
e5b7e9981df3   hello-world   "/hello"   6 hours ago      Exited (0) 6 hours ago                dazzling_knuth
```

Se observan dos contenedores creados a partir de la imagen `hello-world`. Ambos tienen el estado `Exited (0)`, lo que indica que terminaron su ejecución sin errores (el código de salida `0` significa finalización exitosa). El primero corresponde a la ejecución realizada en esta parte del laboratorio, y el segundo a una ejecución anterior. Docker asigna nombres aleatorios (`musing_swirles` y `dazzling_knuth`) cuando no se especifica uno.

### Reflexión

Este comando es útil porque permite ver el historial de contenedores que existen en el sistema, incluso cuando ya no están activos. Gracias a esto se puede confirmar que el contenedor sí se creó y que terminó correctamente.

---

## Diferencia entre `docker ps` y `docker ps -a`

| Comando | Qué muestra |
|---|---|
| `docker ps` | Solo los contenedores que están en ejecución en ese momento. |
| `docker ps -a` | Todos los contenedores, tanto los que están en ejecución como los que ya terminaron o fueron detenidos. |

En este laboratorio, `docker ps` no mostró ningún contenedor, mientras que `docker ps -a` mostró los dos contenedores de `hello-world` que ya habían finalizado.

---

## Preguntas de reflexión

### 1. ¿Qué es la imagen `hello-world`?

Es una imagen oficial muy pequeña que se encuentra en Docker Hub y que se utiliza como prueba de instalación. Contiene un único programa ejecutable (`/hello`) que imprime un mensaje de bienvenida y termina. Su propósito es comprobar que Docker funciona correctamente.

### 2. ¿El contenedor quedó ejecutándose después de imprimir el mensaje?

No. El contenedor ejecuta el programa `/hello`, que imprime el mensaje y finaliza. Un contenedor solo permanece en ejecución mientras su proceso principal siga activo, por lo que al terminar el programa el contenedor se detuvo. Esto se comprobó porque `docker ps` no mostró ningún contenedor activo y `docker ps -a` lo mostró con estado `Exited (0)`.

### 3. ¿Por qué aparece en `docker ps -a` pero no necesariamente en `docker ps`?

Porque `docker ps` solo lista contenedores en ejecución, mientras que `docker ps -a` lista todos los contenedores existentes. El contenedor de `hello-world` ya finalizó, pero no se elimina automáticamente, por lo que sigue registrado en el sistema y solo aparece al usar la opción `-a`.

### 4. ¿Qué demuestra este primer ejemplo sobre Docker?

Demuestra que Docker permite obtener y ejecutar una aplicación de forma sencilla a partir de una imagen, sin instalar manualmente sus dependencias. También muestra que un contenedor es un proceso que vive mientras su programa principal se ejecuta, y que la imagen y el contenedor son cosas distintas: la misma imagen `hello-world` se usó para crear dos contenedores diferentes. Finalmente, confirma que la instalación de Docker, corregida en la Parte 1, funciona de extremo a extremo.

---

## Reflexión personal

Con esta parte pude ver el ciclo básico de un contenedor: se toma una imagen, se crea un contenedor a partir de ella, este ejecuta su tarea y luego termina, quedando registrado en el sistema. También entendí que un contenedor no tiene que seguir corriendo para existir, y que `docker ps -a` es necesario para ver contenedores que ya finalizaron.