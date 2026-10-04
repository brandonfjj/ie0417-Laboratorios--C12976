# Laboratorio 1 — Git, Doxygen y Sphinx

**Estudiante:** Brandon Fuentes Jiménez
**Carné:** C12976
**Curso:** IE0417 — Diseño de Software para Ingeniería

## Repositorio y sitio publicado

- **Repositorio de entrega:** https://github.com/brandonfjj/ie0417-laboratorio-1-git-documentacion-C12976
- **Sitio publicado (portada):** https://ie0417-laboratorio-1-git-documentacio.netlify.app/
- **Documentación C++ (LevelDB, generada con Doxygen):** https://ie0417-laboratorio-1-git-documentacio.netlify.app/cpp/
- **Documentación Python (Rich, generada con Sphinx):** https://ie0417-laboratorio-1-git-documentacio.netlify.app/python/

## Proyectos seleccionados

| Proyecto | Lenguaje | Herramienta | Repositorio original |
|---|---|---|---|
| LevelDB | C++ | Doxygen | https://github.com/google/leveldb |
| Rich | Python | Sphinx | https://github.com/Textualize/rich |

El detalle de la selección (licencia, commit exacto, métricas de `cloc`) está en `doxygen/seleccion.md` y `sphinx/seleccion.md`.

## Cómo regenerar la documentación

Ambas documentaciones se generan a partir del código fuente de los proyectos originales, clonado *fuera* de este repositorio, en el commit exacto registrado en cada `seleccion.md`. Este repositorio no contiene el código fuente de LevelDB ni de Rich, solo la configuración y el HTML ya generado.

### Doxygen (LevelDB)

```bash
# 1. Clonar LevelDB en una carpeta de trabajo separada
git clone https://github.com/google/leveldb.git /ruta/a/leveldb
cd /ruta/a/leveldb
git checkout 7ee830d02b623e8ffe0b95d59a74db1e58da04c5

# 2. Instalar Doxygen y Graphviz
sudo apt-get install -y doxygen graphviz

# 3. Ajustar las rutas INPUT/EXCLUDE/OUTPUT_DIRECTORY en doxygen/Doxyfile
#    para que apunten a la carpeta donde se clonó LevelDB (paso 1)

# 4. Generar la documentación desde la carpeta doxygen/ de este repositorio
cd doxygen
doxygen Doxyfile 2>&1 | tee build.log
```

El HTML resultante se genera directamente en `site/cpp/`.

### Sphinx (Rich)

```bash
# 1. Clonar Rich en una carpeta de trabajo separada
git clone https://github.com/Textualize/rich.git /ruta/a/rich
cd /ruta/a/rich
git checkout 9d8f9a372cc5916fd4781fec207ced7ddac2f08f

# 2. Crear un entorno virtual exclusivo e instalar dependencias
python -m venv .venv-rich-docs
source .venv-rich-docs/bin/activate
pip install -r sphinx/requirements-docs.txt

# 3. Ajustar sys.path en sphinx/source/conf.py para que apunte
#    a la carpeta donde se clonó Rich (paso 1)

# 4. Regenerar el árbol de API (opcional, solo si cambia la estructura de módulos)
sphinx-apidoc -o sphinx/source/api /ruta/a/rich/rich

# 5. Generar el HTML
sphinx-build -b html -d /tmp/sphinx-doctrees sphinx/source site/python 2>&1 | tee sphinx/build.log
```

## Versión de las herramientas utilizadas

| Herramienta | Versión |
|---|---|
| Doxygen | `1.9.1` |
| Graphviz (dot) | `2.43.0 (0)` |
| Python | `3.10.12` |
| Sphinx | `4.3.2` |
| Git | `2.34.1` |
| cloc | `1.90` |

## Estructura del repositorio

```text

├── README.md
├── doxygen
│   ├── Doxyfile
│   ├── build.log
│   ├── evidencias
│   ├── mainpage.md
│   └── seleccion.md
├── git
│   ├── evidencias
│   └── learn-git-branching.md
├── informe.md
├── scripts
├── site
│   ├── cpp
│   ├── index.html
│   └── python
└── sphinx
    ├── Makefile
    ├── build
    ├── build.log
    ├── evidencias
    ├── make.bat 
    ├── requirements-docs.txt
    ├── seleccion.md
    └── source

```