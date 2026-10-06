# Parte 5: Crear una aplicación sencilla

## Qué se hizo

Dentro de la carpeta `App/` se crearon los archivos `app.py` y `requirements.txt`, se instalaron las dependencias y se ejecutó la aplicación localmente.

## Archivos creados

### `app.py`

```python
from flask import Flask
import os

app = Flask(__name__)

@app.route("/")
def home():
    mensaje = os.environ.get("MENSAJE", "Hola desde Flask en Docker")
    return f"""
    <h1>{mensaje}</h1>
    <p>Esta aplicación se está ejecutando dentro de un contenedor.</p>
    """

@app.route("/info")
def info():
    return {
        "app": "Laboratorio de contenedores",
        "curso": "IE0417",
        "tema": "Docker"
    }

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
```

### `requirements.txt`

```text
flask
```

## Comandos ejecutados

| Comando | Para qué sirve |
|---|---|
| `pip install -r requirements.txt` | Instala las dependencias listadas en `requirements.txt`. |
| `python3 app.py` | Ejecuta la aplicación con el intérprete de Python 3. |

## Resultado obtenido

Instalación de dependencias:

```text
$ pip install -r requirements.txt
Defaulting to user installation because normal site-packages is not writeable
Collecting flask
  Downloading flask-3.1.3-py3-none-any.whl (103 kB)
Requirement already satisfied: click>=8.1.3 in /home/brandonfj/.local/lib/python3.10/site-packages (from flask->-r requirements.txt (line 1)) (8.3.1)
Collecting markupsafe>=2.1.1
  Downloading markupsafe-3.0.4-cp310-cp310-manylinux2014_x86_64.manylinux_2_17_x86_64.manylinux_2_28_x86_64.whl (20 kB)
Collecting werkzeug>=3.1.0
  Downloading werkzeug-3.1.9-py3-none-any.whl (228 kB)
Collecting jinja2>=3.1.2
  Using cached jinja2-3.1.6-py3-none-any.whl (134 kB)
Collecting itsdangerous>=2.2.0
  Downloading itsdangerous-2.2.0-py3-none-any.whl (16 kB)
Collecting blinker>=1.9.0
  Downloading blinker-1.9.0-py3-none-any.whl (8.5 kB)
Installing collected packages: markupsafe, itsdangerous, blinker, werkzeug, jinja2, flask
Successfully installed blinker-1.9.0 flask-3.1.3 itsdangerous-2.2.0 jinja2-3.1.6 markupsafe-3.0.4 werkzeug-3.1.9
```

Ejecución de la aplicación y acceso desde el navegador en `http://localhost:5000`:

```text
$ python3 app.py
 * Serving Flask app 'app'
 * Debug mode: off
WARNING: This is a development server. Do not use it in a production deployment. Use a production WSGI server instead.
 * Running on all addresses (0.0.0.0)
 * Running on http://127.0.0.1:5000
 * Running on http://172.25.177.44:5000
Press CTRL+C to quit
127.0.0.1 - - [05/Oct/2026 15:38:47] "GET / HTTP/1.1" 200 -
127.0.0.1 - - [05/Oct/2026 15:38:47] "GET /favicon.ico HTTP/1.1" 404 -
127.0.0.1 - - [05/Oct/2026 15:39:08] "GET / HTTP/1.1" 200 -
```

El registro muestra que la ruta `/` respondió con código `200`, es decir, la aplicación funcionó correctamente. La solicitud a `/favicon.ico` devolvió `404` porque la aplicación no define esa ruta; los navegadores la piden automáticamente para mostrar el ícono de la pestaña, por lo que no representa un error de la aplicación.


## Qué hace la aplicación

La aplicación es un servidor web mínimo hecho con Flask. Al acceder a la ruta principal muestra una página con un mensaje y un texto que indica que la aplicación se está ejecutando dentro de un contenedor. El mensaje se toma de la variable de entorno `MENSAJE`; si esta no está definida, se usa el valor por defecto `"Hola desde Flask en Docker"`. Esto permite cambiar el mensaje sin modificar el código, por ejemplo al ejecutar el contenedor más adelante.

## Rutas de la aplicación

| Ruta | Respuesta |
|---|---|
| `/` | Página HTML con el mensaje (de la variable `MENSAJE` o el valor por defecto) y un texto indicando que la aplicación corre en un contenedor. |
| `/info` | Un objeto JSON con el nombre de la aplicación, el curso y el tema: `{"app": "Laboratorio de contenedores", "curso": "IE0417", "tema": "Docker"}`. Flask convierte automáticamente un diccionario de Python en una respuesta JSON [1]. |

## Dependencia utilizada

La aplicación utiliza únicamente **Flask**, un *framework* web para Python, declarado en `requirements.txt`. Al instalarlo, `pip` también instaló sus dependencias (Werkzeug, Jinja2, Click, ItsDangerous, Blinker y MarkupSafe), como se observa en la salida de la instalación.

## Por qué se usa `host="0.0.0.0"` en lugar de `localhost`

Por defecto, el servidor de desarrollo de Flask solo es accesible desde el mismo equipo donde se ejecuta. Para hacerlo accesible desde otras máquinas de la red se debe indicar `0.0.0.0` como host, lo cual hace que escuche en todas las interfaces de red [1]. Esto se refleja en el mensaje `Running on all addresses (0.0.0.0)` al iniciar la aplicación, donde además se muestran la dirección local `127.0.0.1` y la dirección de red `172.25.177.44`.

Esto es especialmente importante en un contenedor. Cada contenedor tiene su propio entorno de red aislado, creado por Docker mediante *namespaces* [2], por lo que `localhost` dentro del contenedor se refiere solo al propio contenedor. Para acceder a la aplicación desde el equipo anfitrión, Docker debe reenviar el tráfico desde un puerto del host hacia el puerto del contenedor (publicación de puertos) [3]. Si la aplicación escuchara solo en `localhost` dentro del contenedor, ese tráfico reenviado no la alcanzaría. Esta última conclusión la deduzco de la combinación de las referencias citadas, no de una afirmación textual de ellas.

## Preguntas de reflexión

### 1. ¿Qué hace Flask en esta aplicación?

Flask es el *framework* que permite crear el servidor web. Se usa para crear la aplicación (`Flask(__name__)`), asociar rutas con funciones mediante el decorador `@app.route`, y generar las respuestas HTTP (HTML en `/` y JSON en `/info`). También incluye un servidor de desarrollo incorporado, que es el que se inicia con `app.run()`. Este servidor es útil para pruebas, pero no está pensado para producción [1], como lo advierte el mensaje `WARNING: This is a development server` que apareció al ejecutar la aplicación.

### 2. ¿Para qué sirve el archivo `requirements.txt`?

Es un archivo que lista las dependencias que `pip` debe instalar, una por línea [4]. Con `pip install -r requirements.txt` se instalan todas de una vez, lo que permite reproducir el entorno en otra máquina o, más adelante, dentro de la imagen de Docker sin instalar los paquetes manualmente.

### 3. ¿Por qué una aplicación dentro de un contenedor debe escuchar en `0.0.0.0`?

Porque dentro del contenedor, `localhost` solo se refiere al propio contenedor. Si la aplicación escucha únicamente ahí, no puede recibir el tráfico que Docker reenvía desde el puerto publicado del host [3]. Al escuchar en `0.0.0.0`, la aplicación acepta conexiones por todas las interfaces del contenedor, incluida la que Docker usa para conectarlo con el exterior.

### 4. ¿Qué diferencia hay entre ejecutar la aplicación localmente y ejecutarla dentro de Docker?

Localmente, la aplicación usa el Python y las bibliotecas instaladas en mi propio sistema (en este caso, Python 3.10 en Ubuntu sobre WSL2 y Flask instalado con `pip` en mi usuario), por lo que depende de que mi entorno esté correctamente configurado. Dentro de Docker, la aplicación se ejecuta en un contenedor con su propio sistema de archivos, su propia versión de Python y sus dependencias instaladas desde `requirements.txt`, independiente de lo que haya instalado en mi equipo. Además, la red del contenedor está aislada, por lo que para acceder a la aplicación desde el navegador hay que publicar un puerto con `-p`, algo que no es necesario al ejecutarla localmente [3].

## Reflexión personal

Con esta parte vi que el código de la aplicación es sencillo, pero el entorno en que se ejecuta importa mucho: los errores que aparecieron (`python` que no existe, la indentación) dependen de mi equipo, y justamente eso es lo que Docker busca evitar al empaquetar la aplicación con sus dependencias. También entendí que `host="0.0.0.0"` no es un detalle menor, porque en un contenedor es la diferencia entre que la aplicación sea accesible o no desde afuera.

# Parte 6: Construccion de una imagen con Dockerfile

## Qué se hizo

En la carpeta `app/` se creó el archivo `Dockerfile`, se construyó la imagen `laboratorio-flask:1.0`, se ejecutó un contenedor llamado `app-lab` a partir de ella y finalmente se detuvo y se eliminó.

## Dockerfile

```dockerfile
FROM python:3.11-slim
WORKDIR /app
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt
COPY . .
EXPOSE 5000
CMD ["python", "app.py"]
```

## Documentación de cada instrucción

### `FROM python:3.11-slim`

Define la imagen base sobre la que se construye la nueva imagen. Todas las instrucciones siguientes se aplican encima de ella [5]. En este caso se usa la imagen oficial de Python 3.11 en su variante `slim`, que ya incluye el intérprete de Python [9]. Por eso en la salida del *build* aparece `Pulling from library/python`: como la imagen base no estaba en el equipo, Docker la descargó como primer paso.

### `WORKDIR /app`

Establece el directorio de trabajo para las instrucciones siguientes (`COPY`, `RUN`, `CMD`). Si el directorio no existe, Docker lo crea [5]. Así, los archivos de la aplicación quedan en `/app` dentro de la imagen.

### `COPY requirements.txt .`

Copia el archivo `requirements.txt` desde la carpeta `app/` del equipo (el contexto de construcción) hacia el directorio de trabajo de la imagen (`/app`) [5]. Se copia solo este archivo, antes que el resto del código, por la razón explicada en la pregunta 3.

### `RUN pip install --no-cache-dir -r requirements.txt`

Ejecuta un comando **durante la construcción** de la imagen, y el resultado queda guardado en una capa de la imagen [5]. Aquí instala Flask y sus dependencias. La opción `--no-cache-dir` le indica a `pip` que no guarde su caché de descargas, lo que evita que esos archivos ocupen espacio adicional en la imagen.

### `EXPOSE 5000`

Documenta que la aplicación escucha en el puerto 5000 dentro del contenedor. **No publica el puerto**: para que sea accesible desde el equipo anfitrión hay que usar `-p` (o `-P`) al ejecutar el contenedor [5], [10]. Esto se puede ver en `docker ps`, donde la columna `PORTS` muestra solo `5000/tcp`, sin ninguna asignación hacia un puerto del host.

### `CMD ["python", "app.py"]`

Define el comando por defecto que se ejecuta cuando se inicia un contenedor a partir de la imagen. No se ejecuta durante la construcción [5]. Está escrito en formato de lista (forma *exec*), y solo puede haber un `CMD` por Dockerfile; si hubiera varios, solo tendría efecto el último [5].

## Comandos ejecutados

| Comando | Para qué sirve |
|---|---|
| `docker build -t laboratorio-flask:1.0 .` | Construye una imagen a partir del Dockerfile de la carpeta actual (el `.` final es el contexto de construcción) y le asigna el nombre `laboratorio-flask` y la etiqueta `1.0` mediante `-t` [6]. |
| `docker images` | Lista las imágenes locales. |
| `docker run --name app-lab laboratorio-flask:1.0` | Crea e inicia un contenedor llamado `app-lab` a partir de la imagen. |
| `docker ps` | Lista los contenedores en ejecución (se ejecutó en otra terminal). |
| `docker stop app-lab` | Detiene el contenedor. |
| `docker rm app-lab` | Elimina el contenedor. |

## Resultado obtenido

### Construcción de la imagen

```text
$ docker build -t laboratorio-flask:1.0 .
DEPRECATED: The legacy builder is deprecated and will be removed in a future release.
            Install the buildx component to build images with BuildKit:
            https://docs.docker.com/go/buildx/

Sending build context to Docker daemon  4.096kB
Step 1/7 : FROM python:3.11-slim
3.11-slim: Pulling from library/python
48355dfcbf9e: Pulling fs layer
295f1967d045: Pulling fs layer
6b37362b3da7: Pulling fs layer
f64163c1b799: Pulling fs layer
1da39cd6a499: Download complete
48355dfcbf9e: Download complete
80152a8a1b45: Download complete
f64163c1b799: Download complete
295f1967d045: Download complete
6b37362b3da7: Download complete
6b37362b3da7: Pull complete
f64163c1b799: Pull complete
295f1967d045: Pull complete
48355dfcbf9e: Pull complete
Digest: sha256:6f31d6e9ba2b0a787a3f81c37b004155b87b9efa1b771182bd550c1615745be5
Status: Downloaded newer image for python:3.11-slim
 ---> 6f31d6e9ba2b
Step 2/7 : WORKDIR /app
 ---> Running in 83cc4ce356c4
 ---> Removed intermediate container 83cc4ce356c4
 ---> afcbaa56c851
Step 3/7 : COPY requirements.txt .
 ---> a612ae74ab51
Step 4/7 : RUN pip install --no-cache-dir -r requirements.txt
 ---> Running in b021c2008ba2
Collecting flask (from -r requirements.txt (line 1))
  Downloading flask-3.1.3-py3-none-any.whl.metadata (3.2 kB)
Collecting blinker>=1.9.0 (from flask->-r requirements.txt (line 1))
  Downloading blinker-1.9.0-py3-none-any.whl.metadata (1.6 kB)
Collecting click>=8.1.3 (from flask->-r requirements.txt (line 1))
  Downloading click-8.5.0-py3-none-any.whl.metadata (2.6 kB)
Collecting itsdangerous>=2.2.0 (from flask->-r requirements.txt (line 1))
  Downloading itsdangerous-2.2.0-py3-none-any.whl.metadata (1.9 kB)
Collecting jinja2>=3.1.2 (from flask->-r requirements.txt (line 1))
  Downloading jinja2-3.1.6-py3-none-any.whl.metadata (2.9 kB)
Collecting markupsafe>=2.1.1 (from flask->-r requirements.txt (line 1))
  Downloading markupsafe-3.0.4-cp311-cp311-manylinux2014_x86_64.manylinux_2_17_x86_64.manylinux_2_28_x86_64.whl.metadata (2.7 kB)
Collecting werkzeug>=3.1.0 (from flask->-r requirements.txt (line 1))
  Downloading werkzeug-3.1.9-py3-none-any.whl.metadata (4.1 kB)
Downloading flask-3.1.3-py3-none-any.whl (103 kB)
   ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ 103.4/103.4 kB 1.1 MB/s eta 0:00:00
Downloading blinker-1.9.0-py3-none-any.whl (8.5 kB)
Downloading click-8.5.0-py3-none-any.whl (125 kB)
   ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ 125.3/125.3 kB 3.8 MB/s eta 0:00:00
Downloading itsdangerous-2.2.0-py3-none-any.whl (16 kB)
Downloading jinja2-3.1.6-py3-none-any.whl (134 kB)
   ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ 134.9/134.9 kB 6.0 MB/s eta 0:00:00
Downloading markupsafe-3.0.4-cp311-cp311-manylinux2014_x86_64.manylinux_2_17_x86_64.manylinux_2_28_x86_64.whl (22 kB)
Downloading werkzeug-3.1.9-py3-none-any.whl (228 kB)
   ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ 228.7/228.7 kB 6.7 MB/s eta 0:00:00
Installing collected packages: markupsafe, itsdangerous, click, blinker, werkzeug, jinja2, flask
Successfully installed blinker-1.9.0 click-8.5.0 flask-3.1.3 itsdangerous-2.2.0 jinja2-3.1.6 markupsafe-3.0.4 werkzeug-3.1.9
WARNING: Running pip as the 'root' user can result in broken permissions and conflicting behaviour with the system package manager. It is recommended to use a virtual environment instead: https://pip.pypa.io/warnings/venv

[notice] A new release of pip is available: 24.0 -> 26.2.1
[notice] To update, run: pip install --upgrade pip
 ---> Removed intermediate container b021c2008ba2
 ---> 5e2916caacec
Step 5/7 : COPY . .
 ---> df709a0d1a68
Step 6/7 : EXPOSE 5000
 ---> Running in 2ba58a137730
 ---> Removed intermediate container 2ba58a137730
 ---> 4442b4ac94f1
Step 7/7 : CMD ["python", "app.py"]
 ---> Running in 64299c3a6d04
 ---> Removed intermediate container 64299c3a6d04
 ---> b1b6522d36f1
Successfully built b1b6522d36f1
Successfully tagged laboratorio-flask:1.0
```

La construcción terminó con `Successfully built` y `Successfully tagged laboratorio-flask:1.0`. Se observan siete pasos, uno por cada instrucción del Dockerfile, ejecutados en orden. Además:

- **Mensaje `DEPRECATED`:** indica que se usó el constructor clásico de Docker (*legacy builder*) y que se recomienda instalar el componente `buildx` para usar BuildKit. Es una advertencia y no impidió la construcción.
- **Advertencias de `pip`:** el aviso de ejecutar `pip` como `root` y el aviso de nueva versión de `pip` son advertencias, no errores. Los paquetes se instalaron dentro de la imagen, no en el sistema del equipo.
- **Versiones instaladas:** dentro de la imagen se instaló `click-8.5.0`, mientras que en la instalación local de la Parte 5 `pip` reportó que ya existía `click` 8.3.1. Ambos cumplen el requisito `click>=8.1.3` de Flask, pero no son la misma versión.

### Lista de imágenes

```text
$ docker images
IMAGE                   ID             DISK USAGE   CONTENT SIZE   EXTRA
hello-world:latest      5e2309035332       25.9kB         9.49kB    U
laboratorio-flask:1.0   b1b6522d36f1        222MB         54.3MB
python:3.11-slim        6f31d6e9ba2b        200MB         50.8MB
ubuntu:latest           f144425ff09b        162MB         45.6MB    U
```

La imagen `laboratorio-flask:1.0` aparece con el ID `b1b6522d36f1`, el mismo que reportó el final del *build*. Ocupa 222 MB en disco, unos 22 MB más que la imagen base `python:3.11-slim` (200 MB), que corresponde a Flask y sus dependencias más los archivos de la aplicación. La imagen `python:3.11-slim` aparece en la lista porque se descargó durante el *build*. La marca `U` (en uso) aparece solo en `hello-world` y `ubuntu`, que tienen contenedores asociados de partes anteriores; `laboratorio-flask:1.0` aún no tenía ninguno cuando se ejecutó este comando.

### Ejecución del contenedor

```text
$ docker run --name app-lab laboratorio-flask:1.0
 * Serving Flask app 'app'
 * Debug mode: off
WARNING: This is a development server. Do not use it in a production deployment. Use a production WSGI server instead.
 * Running on all addresses (0.0.0.0)
 * Running on http://127.0.0.1:5000
 * Running on http://172.17.0.2:5000
Press CTRL+C to quit
```

El contenedor se ejecutó en primer plano, por lo que la terminal quedó ocupada mostrando los mensajes de Flask; por eso `docker ps` se ejecutó en otra terminal. La dirección de red `172.17.0.2` es la IP interna del contenedor y es distinta de la `172.25.177.44` que mostró Flask al ejecutarse localmente en la Parte 5, lo que confirma que el contenedor tiene su propia red.

### Contenedores en ejecución, detención y eliminación

```text
$ docker ps
CONTAINER ID   IMAGE                   COMMAND           CREATED          STATUS          PORTS      NAMES
43afd3ae5770   laboratorio-flask:1.0   "python app.py"   29 seconds ago   Up 29 seconds   5000/tcp   app-lab

$ docker stop app-lab
app-lab

$ docker rm app-lab
app-lab
```

El contenedor `app-lab` aparece con estado `Up`, usando el comando `python app.py` definido en el `CMD`. La columna `PORTS` muestra `5000/tcp` sin asignación al host, porque en el `docker run` no se usó `-p`; en esta parte no se probó el acceso desde el navegador.

## Qué significa construir una imagen

Construir una imagen es el proceso en el que Docker lee las instrucciones del Dockerfile, las ejecuta en orden y guarda el resultado como una imagen [5]. Cada instrucción corresponde aproximadamente a una capa de la imagen [7]. En este laboratorio, el resultado fue una imagen que contiene Python, Flask y el código de la aplicación, lista para crear contenedores. El `.` al final del comando indica el contexto de construcción, es decir, la carpeta cuyos archivos están disponibles para instrucciones como `COPY` [6]. En la salida se ve como `Sending build context to Docker daemon 4.096kB`.

## Qué significa el nombre `laboratorio-flask:1.0`

Es el nombre de la imagen, con el formato `repositorio:etiqueta` [6]. `laboratorio-flask` es el nombre (repositorio) y `1.0` es la etiqueta (*tag*), que sirve para identificar una versión de la imagen. Se asigna con la opción `-t` de `docker build` [6]. Si no se especifica etiqueta, Docker usa `latest` por defecto, como ocurrió con `ubuntu:latest` y `hello-world:latest`.

## Diferencia entre el nombre de la imagen y el nombre del contenedor

| | Nombre de la imagen | Nombre del contenedor |
|---|---|---|
| Ejemplo | `laboratorio-flask:1.0` | `app-lab` |
| Qué identifica | La plantilla a partir de la cual se crean contenedores | Una instancia concreta creada a partir de esa imagen |
| Cómo se asigna | Con `-t` en `docker build` | Con `--name` en `docker run` |

En el laboratorio, la imagen `laboratorio-flask:1.0` se usó para crear el contenedor `app-lab`; con esa misma imagen se podrían crear otros contenedores con nombres distintos.

## Resultado de `docker images`

Ver la sección "Lista de imágenes" en los resultados. La imagen aparece como `laboratorio-flask:1.0`, con ID `b1b6522d36f1` y un tamaño en disco de 222 MB.

## Preguntas de reflexión

### 1. ¿Qué es una imagen base?

Es la imagen sobre la que se construye una nueva imagen, y se indica con la instrucción `FROM` [5]. La nueva imagen hereda todo el contenido de la imagen base y le agrega capas encima. En este laboratorio la imagen base es `python:3.11-slim`, que ya trae el sistema de archivos de Linux y el intérprete de Python [9], de modo que solo hizo falta agregar Flask y el código de la aplicación.

### 2. ¿Por qué se usa una imagen `slim`?

Porque la variante `slim` solo incluye los paquetes mínimos de Debian necesarios para ejecutar Python, y no los paquetes adicionales que trae la imagen por defecto [9]. Esto produce una imagen más pequeña, que se descarga más rápido y ocupa menos espacio. La documentación de la imagen advierte que `slim` tampoco incluye los paquetes necesarios para compilar extensiones de otros lenguajes [9]. Para esta aplicación no hizo falta, ya que Flask y sus dependencias se instalaron como paquetes ya compilados (archivos `.whl`), como se ve en la salida del *build*.

### 3. ¿Por qué se copian primero las dependencias y luego el resto del código?

Por el uso de la caché de construcción. Cada instrucción genera una capa, y si una capa cambia, esa capa y todas las siguientes deben reconstruirse; si no cambia, Docker la reutiliza de la caché [7]. La documentación de Docker recomienda ordenar las instrucciones de la menos cambiante a la más cambiante [8]. El archivo `requirements.txt` cambia pocas veces, mientras que el código de `app.py` cambia con frecuencia. Al copiar primero solo `requirements.txt` y ejecutar `pip install`, esa capa se reutiliza aunque se modifique el código, y solo se vuelve a ejecutar desde `COPY . .` en adelante [7]. Si se hiciera `COPY . .` primero, cualquier cambio en el código invalidaría la caché y obligaría a reinstalar las dependencias en cada construcción.

En este laboratorio se construyó la imagen una sola vez, por lo que no se llegó a comprobar el efecto de la caché. Para verlo, bastaría con modificar `app.py` y volver a construir.

### 4. ¿Qué diferencia hay entre `RUN` y `CMD`?

`RUN` ejecuta un comando durante la construcción de la imagen y guarda el resultado en una capa; `CMD` no ejecuta nada al construir, sino que define el comando por defecto que se ejecutará cuando se inicie un contenedor [5]. Además, el `CMD` puede reemplazarse al pasar argumentos en `docker run`, y solo puede haber uno por Dockerfile [5].

En este laboratorio se vio claramente: la salida de `pip install` (`RUN`) apareció en el `docker build`, mientras que la salida de Flask (`CMD`) apareció en el `docker run`. También se nota que dentro de la imagen el comando es `python` y no `python3`, porque la imagen de Python sí incluye ese ejecutable, a diferencia de lo que ocurrió en el sistema local en la Parte 5.

### 5. ¿Qué pasaría si se elimina la imagen pero no el Dockerfile?

No se perdería la posibilidad de recuperar la imagen: el Dockerfile contiene las instrucciones para construirla, y Docker puede reconstruirla leyéndolas con `docker build` [5]. Para eso también harían falta los archivos que el Dockerfile copia (`requirements.txt` y el código de la aplicación) y acceso a internet para volver a descargar la imagen base y las dependencias.

Sin embargo, la imagen reconstruida podría no ser idéntica a la original. En `requirements.txt` solo se indica `flask`, sin versión, y `pip` instala la última versión disponible cuando no se especifica una [11]. Prueba de esto es que `click` se instaló en la versión 8.5.0 dentro de la imagen y estaba en 8.3.1 en el sistema local. Fijar las versiones en `requirements.txt` (por ejemplo, `flask==3.1.3`) haría la construcción más reproducible.

## Reflexión personal

Con esta parte entendí que el Dockerfile es la receta y la imagen es el resultado de aplicarla, y que el orden de las instrucciones no es arbitrario: afecta cuánto se reutiliza en cada construcción. También me llamó la atención que, aunque la aplicación es la misma de la Parte 5, ahora corre en su propio entorno con su propio Python y su propia red, y que todavía no es accesible desde el navegador porque falta publicar el puerto. Por último, ver que las versiones instaladas dentro de la imagen no coincidían exactamente con las de mi sistema me mostró por qué conviene fijar las versiones de las dependencias.

## Referencias (continuación)

[5] Docker Inc., "Dockerfile reference," *Docker Docs*. [En línea]. Disponible en: https://docs.docker.com/reference/dockerfile/. [Accedido: 5-oct-2026].

[6] Docker Inc., "Build, tag, and publish an image," *Docker Docs*. [En línea]. Disponible en: https://docs.docker.com/get-started/docker-concepts/building-images/build-tag-and-publish-an-image/. [Accedido: 5-oct-2026].

[7] Docker Inc., "Layers," *Docker Build guide, Docker Docs*. [En línea]. Disponible en: https://docs.docker.com/build/guide/layers. [Accedido: 5-oct-2026].

[8] Docker Inc., "Build cache invalidation," *Docker Docs*. [En línea]. Disponible en: https://docs.docker.com/build/cache/invalidation. [Accedido: 5-oct-2026].

[9] Docker Inc., "python — Docker Official Image," *Docker Hub*. [En línea]. Disponible en: https://hub.docker.com/_/python. [Accedido: 5-oct-2026].

[10] Docker Inc., "Port publishing and mapping," *Docker Docs*. [En línea]. Disponible en: https://docs.docker.com/engine/network/port-publishing/. [Accedido: 5-oct-2026].

[11] Python Packaging Authority, "User Guide," *pip Documentation*. [En línea]. Disponible en: https://pip.pypa.io/en/stable/user_guide/. [Accedido: 5-oct-2026].