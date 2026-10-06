# Laboratorio 2: Introducción práctica a contenedores con Docker

**Universidad de Costa Rica**  
**Escuela de Ingeniería Eléctrica**  
**IE0417 Diseño de Software para Ingeniería**  
**Estudiante:** Brandon Fuentes Jiménez - C12976

## Índice

1. [Parte 1: Verificación de instalación de Docker](parte1-verificacion.md)
2. [Parte 2: Primer contenedor](parte2-comandos-basicos.md)
3. [Parte 3: Imágenes y contenedores](parte3-imagenes-y-contenedores.md)
4. [Parte 4: Administración básica de contenedores](parte3-imagenes-y-contenedores.md#administracion-de-contenedores)
5. [Parte 5: Crear una aplicación sencilla](parte4-dockerfile.md)
6. [Parte 6: Construir una imagen con Dockerfile](parte4-dockerfile.md#parte-6-construccion-de-una-imagen-con-dockerfile)
7. [Parte 7: Publicación de puertos](parte5-puertos.md)
8. [Parte 8: Logs e inspección de contenedores](parte5-puertos.md#logs-e-inspeccion)
9. [Parte 9: Variables de entorno](parte5-puertos.md#variables-de-entorno)
10. [Parte 10: Persistencia con volúmenes](parte6-volumenes.md)
11. [Parte 11: Bind mounts](parte6-volumenes.md#bind-mounts)
12. [Parte 12: Redes de Docker](parte7-redes.md)
13. [Parte 13: Comunicación entre servicios](parte7-redes.md#comunicacion-entre-servicios)
14. [Parte 14: Limpieza del ambiente](parte8-limpieza.md)
15. [Reflexión final](#reflexión-final)

## Estructura del proyecto

```text
laboratorio-contenedores/
├── README.md
├── parte1-verificacion.md
├── parte2-comandos-basicos.md
├── parte3-imagenes-y-contenedores.md
├── parte4-dockerfile.md
├── parte5-puertos.md
├── parte6-volumenes.md
├── parte7-redes.md
├── parte8-limpieza.md
└── app/
    ├── Dockerfile
    ├── app.py
    └── requirements.txt
```
## Reflexión final

### 1. ¿Qué es un contenedor?
Un contenedor es un entorno aislado en el que se puede ejecutar una aplicación junto con los elementos que necesita para funcionar. A diferencia de una máquina virtual, no necesita ejecutar un sistema operativo completo, sino que comparte el kernel del sistema anfitrión. Esto hace que los contenedores sean más ligeros y rápidos de crear y ejecutar.

### 2. ¿Qué problema resuelve Docker?
Docker facilita la creación de entornos de ejecución reproducibles para las aplicaciones. Permite empaquetar una aplicación junto con sus dependencias y configuración para que pueda ejecutarse de una manera similar en diferentes equipos. Esto ayuda a evitar problemas causados por diferencias entre los entornos de desarrollo y ejecución.

### 3. ¿Qué diferencia hay entre una imagen y un contenedor?
Una imagen es una plantilla que contiene los elementos necesarios para crear un entorno de ejecución. El contenedor es una instancia creada a partir de esa imagen. Por ejemplo, una misma imagen de Ubuntu puede utilizarse para crear varios contenedores independientes.

### 4. ¿Qué diferencia hay entre un contenedor y una máquina virtual?
Una máquina virtual incluye un sistema operativo completo que se ejecuta sobre un hipervisor, mientras que un contenedor comparte el kernel del sistema anfitrión. Por esta razón, los contenedores normalmente utilizan menos recursos y pueden iniciarse más rápidamente que las máquinas virtuales.

### 5. ¿Qué aprendió sobre puertos?
Aprendí que el puerto utilizado por una aplicación dentro del contenedor no necesariamente es directamente accesible desde la máquina anfitriona. Para permitir el acceso se puede realizar un mapeo entre un puerto del host y uno del contenedor, por ejemplo con `-p 8080:5000`. En este caso, el puerto 8080 pertenece al host y el 5000 al contenedor.

También comprendí que se pueden utilizar diferentes puertos del host para acceder al mismo puerto de la aplicación dentro de distintos contenedores.

### 6. ¿Qué aprendió sobre volúmenes?
Aprendí que los datos almacenados directamente dentro de un contenedor pueden desaparecer cuando este es eliminado. Los volúmenes permiten mantener información independientemente del ciclo de vida de un contenedor, por lo que son útiles para datos que deben persistir.

También aprendí la diferencia entre un volumen administrado por Docker y un *bind mount*. Los *bind mounts* permiten utilizar directamente archivos o carpetas del equipo anfitrión dentro del contenedor, lo cual resulta especialmente útil durante el desarrollo.

### 7. ¿Qué aprendió sobre redes?
Aprendí que Docker permite crear redes para que diferentes contenedores puedan comunicarse entre sí. Cuando varios contenedores están conectados a la misma red, pueden utilizar el nombre de otro contenedor para establecer una conexión, en lugar de depender directamente de una dirección IP.

Esto permite separar diferentes servicios de una aplicación y hacer que se comuniquen entre ellos de una manera más organizada.

### 8. ¿En qué casos usaría Docker en un proyecto de software?
Usaría Docker cuando un proyecto tenga varias dependencias o servicios y sea importante que todos los integrantes del equipo trabajen con un entorno similar. También lo utilizaría para ejecutar aplicaciones junto con servicios adicionales, como bases de datos, sin tener que instalar cada componente directamente en el sistema operativo.

Además, Docker puede ser útil para desplegar aplicaciones y mantener una configuración consistente entre desarrollo, pruebas y producción.

### 9. ¿Qué parte del laboratorio le pareció más útil?
La parte que me pareció más útil fue la construcción y ejecución de la aplicación Flask dentro de Docker, especialmente cuando se combinaron el Dockerfile, el mapeo de puertos, las variables de entorno y los volúmenes.

Esta parte permitió relacionar varios conceptos del laboratorio con una aplicación real, en lugar de trabajar solamente con contenedores de prueba.

### 10. ¿Qué parte le pareció más confusa?
La parte que me pareció más confusa al inicio fue la diferencia entre los puertos del host y los puertos del contenedor, ya que una misma aplicación podía utilizar el puerto 5000 dentro del contenedor y ser accesible mediante otro puerto, como el 8080, desde el host.

También fue necesario comprender la diferencia entre el almacenamiento interno de un contenedor, los volúmenes y los *bind mounts*. Después de realizar las pruebas de persistencia y modificar archivos desde el host, la diferencia entre estos mecanismos quedó más clara.

