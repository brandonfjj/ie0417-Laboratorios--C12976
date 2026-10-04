# Informe del laboratorio: Control de versiones y documentación automática de software

## 7.1 Justificación de la configuración de Doxygen

Se generó una configuración propia con `doxygen -g Doxyfile` y se modificaron las siguientes opciones respecto a los valores por defecto. No se utilizó ni se copió la configuración de Doxygen que trae el repositorio original de LevelDB

| Opción | Valor utilizado | Justificación |
|---|---|---|
| `PROJECT_NAME` | `"LevelDB"` | Identifica claramente el proyecto documentado en la cabecera de todas las páginas generadas. |
| `PROJECT_NUMBER` | `"commit 7ee830d"` | Deja explícito, en la propia documentación, el commit exacto sobre el que se generó, para que el resultado sea reproducible y trazable frente al `seleccion.md`. |
| `PROJECT_BRIEF` | `"Motor de almacenamiento clave-valor embebido (LSM-Tree)"` | Da contexto inmediato del propósito del proyecto a cualquier persona que abra la portada. |
| `INPUT` | `mainpage.md`, `include/`, `db/`, `table/`, `util/` (rutas absolutas al checkout local) | Se incluyen solo las carpetas con código fuente propio de LevelDB relevantes para la API y su implementación interna, coherente con el conteo de archivos hecho con `cloc` en la sección 6. Se agrega también `mainpage.md` (archivo propio) para que Doxygen lo procese como contenido de portada. |
| `EXCLUDE` | `third_party/` | Excluye las dependencias vendorizadas (`googletest`, `benchmark`), que no son código propio del proyecto y no deben documentarse ni contarse. |
| `RECURSIVE` | `YES` | Necesario porque `db/`, `table/` y `util/` no tienen todos sus archivos en un solo nivel; sin esta opción Doxygen no bajaría a las subcarpetas. |
| `FILE_PATTERNS` | `*.h *.cc *.cpp` | Son las extensiones reales usadas en LevelDB (`.h` para encabezados, `.cc` para implementación); no se usa `.cpp` en el proyecto, pero se deja por compatibilidad general con C++. |
| `EXTRACT_ALL` | `YES` | LevelDB documenta la mayoría de sus símbolos con comentarios `//` en lugar de bloques `/** */` formales. Sin `EXTRACT_ALL`, Doxygen omitiría de la documentación cualquier clase, función o miembro que no tenga un comentario en el formato estricto que reconoce por defecto, dejando fuera partes importantes de la API. |
| `JAVADOC_AUTOBRIEF` | `YES` | Es la opción más crítica de esta configuración: permite que Doxygen interprete la primera línea de los comentarios `//` existentes (por ejemplo, en `include/leveldb/db.h`) como la descripción breve de cada función, en lugar de generar documentación vacía de texto. |
| `GENERATE_HTML` | `YES` | Es el formato de salida exigido por el laboratorio para publicación estática. |
| `HTML_OUTPUT` | `html` | Mantiene la salida HTML organizada en su propia subcarpeta dentro de `OUTPUT_DIRECTORY`, facilitando la publicación posterior. |
| `GENERATE_LATEX` | `NO` | No se requiere salida en PDF/LaTeX para este laboratorio; desactivarlo agiliza la generación. |
| `USE_MDFILE_AS_MAINPAGE` | `mainpage.md` (archivo propio) | Se optó por escribir una portada propia en lugar de usar el `README.md` original de LevelDB, para poder incluir explícitamente la atribución al proyecto, el enlace al repositorio original y el commit documentado, tal como exige la sección 7.2, sin depender del contenido de terceros. |
| `SOURCE_BROWSER` | `YES` | Habilita la exploración del código fuente resaltado directamente desde la documentación, cumpliendo el requisito de "exploración del código fuente o referencias cruzadas". |
| `REFERENCES_LINK_SOURCE` | `YES` | Permite saltar desde una declaración documentada hacia su ubicación exacta en el código fuente navegable. |
| `HAVE_DOT` | `YES` | Habilita el uso de Graphviz, requerido para generar cualquier tipo de diagrama. |
| `CLASS_GRAPH` | `YES` | Genera diagramas de herencia entre clases como `Env` y `EnvWrapper`, o `Iterator` y sus implementaciones concretas, útiles para entender la jerarquía de tipos de LevelDB. |
| `COLLABORATION_GRAPH` | `YES` | Genera diagramas de colaboración que muestran qué clases usa o contiene cada clase (por ejemplo, cómo `DBImpl` se relaciona con `VersionSet`, `MemTable` y `TableCache`), lo cual es más informativo que solo la herencia en un proyecto que usa composición extensivamente. |
| `CALL_GRAPH` / `CALLER_GRAPH` | `YES` | Generan grafos de quién llama a quién a nivel de función, útiles para entender el flujo interno de operaciones como `Get`, `Put` o `Write` sin tener que rastrear manualmente el código. |
| `OUTPUT_DIRECTORY` | Carpeta `site/cpp` dentro del repositorio entregable | Se eligió esta ruta para que el HTML generado quede listo para publicación estática (por ejemplo, en Netlify) sin necesidad de mover archivos manualmente después de cada generación. |

Todas las rutas de `INPUT`, `EXCLUDE` y `OUTPUT_DIRECTORY` apuntan al checkout local de LevelDB fijado en el commit `7ee830d02b623e8ffe0b95d59a74db1e58da04c5`, clonado fuera del repositorio entregable, y no se versiona el código fuente de LevelDB en sí, solo la configuración (`Doxyfile`), el registro de generación (`build.log`) y el HTML resultante.


## 7.2 Advertencias obtenidas durante la generación

La generación finalizó sin errores críticos (el log termina con `finished...`). Se obtuvieron únicamente 4 advertencias, agrupables en dos categorías:

### Categoría 1 — Documentación de símbolos que Doxygen no logra enlazar con su declaración (3 advertencias)

```text
db/fault_injection_test.cc:359: warning: documented symbol 'Status leveldb::FileState::DropUnsyncedData' was not declared or defined.
util/env_posix.cc:808: warning: documented symbol 'leveldb::PosixEnv::PosixEnv' was not declared or defined.
util/env_posix.cc:835: warning: documented symbol 'void leveldb::PosixEnv::BackgroundThreadMain' was not declared or defined.
```

Estas tres advertencias comparten la misma causa raíz: existe un comentario de documentación en el archivo `.cc` para un símbolo (un constructor, un método privado, o una función auxiliar de una clase de prueba) que Doxygen no puede asociar con una declaración correspondiente que haya procesado. Esto ocurre típicamente por alguna de estas razones, comunes en código de sistemas como LevelDB:

- El símbolo pertenece a una clase o método definido únicamente dentro del propio `.cc` (una clase auxiliar local a ese archivo, como suele ocurrir con `FileState` en pruebas de inyección de fallos), sin una declaración visible en ningún header incluido en `INPUT`.
- El símbolo depende de compilación condicional específica de la plataforma POSIX, y Doxygen —al no ejecutar el preprocesador con las mismas macros que un compilador real— puede no resolver correctamente esa declaración.

No se trata de errores de configuración de nuestro `Doxyfile`, sino de un patrón inherente al código fuente de LevelDB al ser analizado estáticamente por una herramienta de documentación sin compilar el proyecto.

### Categoría 2 — Firma de función que no coincide exactamente con la declaración (1 advertencia)

```text
util/env_posix.cc:814: warning: no uniquely matching class member found for 
  void leveldb::PosixEnv::Schedule(void(*background_work_function)(void *background_work_arg), void *background_work_arg)
```

Esta es la advertencia más informativa de las cuatro. Indica que Doxygen sí encontró una declaración de `Schedule` en el header correspondiente, pero la firma exacta (probablemente los nombres de los parámetros del puntero a función) no coincide carácter por carácter con la de la definición en `env_posix.cc`. Es un caso típico cuando una declaración usa nombres de parámetros distintos —o los omite— respecto a su implementación; Doxygen es más estricto que el compilador de C++ al momento de emparejar declaración con definición para heredar la documentación.

### Por qué no se resolvieron

Ninguna de las cuatro advertencias afecta símbolos de la API pública principal (`DB`, `Iterator`, `WriteBatch`, `Options`, etc.), que es la parte que un desarrollador consumidor de la librería necesitaría consultar. Todas involucran a `PosixEnv` (una implementación interna específica de la plataforma) o a clases de utilería de las pruebas (`FileState` en `fault_injection_test.cc`). Se documentan aquí en lugar de ocultarse, siguiendo lo solicitado en el laboratorio, pero no se consideraron críticas para la calidad general de la documentación generada.

## 7.3 Análisis de la documentación Doxygen

**1. Qué muestra la página principal y cómo está organizada la navegación**

La página principal (https://ie0417-laboratorio-1-git-documentacio.netlify.app/cpp/) muestra el contenido de `mainpage.md`, redactado para este laboratorio: la descripción de LevelDB, el repositorio original, la licencia, y el commit documentado. La navegación superior ofrece las pestañas estándar de Doxygen: **Namespaces**, **Classes**, **Files**, más una barra lateral que se expande jerárquicamente por namespace y por clase.

**2. Qué información se genera para clases, estructuras, espacios de nombres, archivos y funciones**

La vista de clases (https://ie0417-laboratorio-1-git-documentacio.netlify.app/cpp/annotated.html) lista todas las clases y structs documentados de LevelDB. Cada clase tiene su propia página. Por ejemplo, `leveldb::DBImpl` (https://ie0417-laboratorio-1-git-documentacio.netlify.app/cpp/classleveldb_1_1dbimpl) que incluye sus structs anidados (`CompactionState`, `Writer`), la lista completa de *Public Member Functions*, y una nota de "Friends" (la clase `DB` tiene acceso amistoso a `DBImpl`). Cada archivo fuente tiene también su propia página, como `arena.cc` (https://ie0417-laboratorio-1-git-documentacio.netlify.app/cpp/arena_8cc.html), con su grafo de inclusión y enlace al código fuente resaltado.

**3. Cómo se presentan parámetros, valores de retorno, miembros, herencia y relaciones**

En la página de `DBImpl`, cada método muestra su firma completa de C++: por ejemplo, `Get(const ReadOptions& options, const Slice& key, std::string* value) override` deja ver el tipo de cada parámetro, el calificador `const`, y que el método sobrescribe (`override`) uno virtual de la clase base `leveldb::DB`. La sección "Public Member Functions inherited from leveldb::DB" muestra explícitamente qué miembros provienen de la clase padre en lugar de repetirlos como propios. La herencia se visualiza con el "Inheritance diagram for leveldb::DBImpl", y las relaciones de composición con el "Collaboration diagram for leveldb::DBImpl" (imagen: https://ie0417-laboratorio-1-git-documentacio.netlify.app/cpp/classleveldb_1_1DBImpl__coll__graph.png).

**4. Qué diagramas o referencias cruzadas se generaron y qué permiten comprender**

Además del diagrama de herencia y colaboración a nivel de clase, cada método individual con lógica no trivial tiene su propio grafo de llamadas ("call graph") o de llamadores ("caller graph"), por ejemplo, el destructor `~DBImpl()` tiene su grafo de llamadas en https://ie0417-laboratorio-1-git-documentacio.netlify.app/cpp/classleveldb_1_1DBImpl_aebabc713c5732cb5f465d58b09176c39_cgraph.png, y el método `Write()` tiene tanto grafo de llamadas como de llamadores. Esto permite entender, sin leer el `.cc`, qué funciones internas invoca cada operación pública y quién más en el código termina llamando a esa función.

**5. Qué parte de la documentación proviene de comentarios estructurados y qué parte puede inferir Doxygen del código**

Las firmas completas, los tipos de parámetros, los calificadores (`override`, `const`, `delete`), y la ubicación exacta en el código fuente ("Definition at line 126 of file db_impl.cc") son inferidos directamente por el analizador sintáctico de Doxygen, sin necesidad de ningún comentario. Los diagramas de herencia, colaboración y llamadas también son completamente inferidos del código. La descripción breve en prosa, cuando aparece, proviene de los comentarios `//` existentes en LevelDB, interpretados gracias a `JAVADOC_AUTOBRIEF`; en el caso concreto de `DBImpl`, se observa que varios métodos (como `Put`, `Delete`, `Write`) no muestran descripción textual adicional más allá de "Implements leveldb::DB", lo que indica que el comentario original en el código no incluía una frase explicativa propia para esa sobreescritura específica.

**6. Qué puede aprender una persona desarrolladora nueva sobre el proyecto gracias a esta documentación**

Puede entender que `DBImpl` es la implementación concreta de la interfaz abstracta `DB`, que expone las operaciones fundamentales (`Get`, `Put`, `Delete`, `Write`, `NewIterator`), y que internamente coordina snapshots (`GetSnapshot`/`ReleaseSnapshot`), compactación (`CompactRange`, y sus variantes `TEST_*` usadas solo en pruebas) y muestreo de lecturas (`RecordReadSample`) — todo esto sin necesidad de leer las más de 1200 líneas de `db_impl.cc`, apoyándose en los grafos de llamadas para entender el flujo interno de, por ejemplo, `Write()`.

**7. Qué elementos quedaron incompletos, poco claros o sin documentar, y por qué**

Como se detalló en 7.2, cuatro símbolos de `PosixEnv` y `FileState` generaron advertencias por no enlazarse correctamente con su declaración. Adicionalmente, en la propia página de `DBImpl` se observa que la mayoría de los métodos públicos carecen de una descripción textual propia más allá de "Implements leveldb::DB" — es decir, Doxygen documenta correctamente la relación de herencia, pero el código original no incluía comentarios `//` explicando el propósito de cada override individual, a diferencia de la documentación más completa presente en la interfaz abstracta `include/leveldb/db.h`.

**Enlaces representativos utilizados en este análisis:**
1. https://ie0417-laboratorio-1-git-documentacio.netlify.app/cpp/
2. https://ie0417-laboratorio-1-git-documentacio.netlify.app/cpp/annotated.html
3. https://ie0417-laboratorio-1-git-documentacio.netlify.app/cpp/classleveldb_1_1dbimpl
4. https://ie0417-laboratorio-1-git-documentacio.netlify.app/cpp/classleveldb_1_1DBImpl__coll__graph.png
5. https://ie0417-laboratorio-1-git-documentacio.netlify.app/cpp/arena_8cc.html



## 8.1 Justificación de la configuración de Sphinx

Se generó una configuración propia de Sphinx (`sphinx/source/conf.py`) partiendo del `conf.py` producido automáticamente, pero completando manualmente las extensiones, opciones de `autodoc`/`napoleon` y el tema, ya que Sphinx no las incluye por defecto. No se utilizó la documentación oficial de Rich ni su configuración original de Sphinx (el proyecto la publica en Read the Docs); se estudió como referencia, pero la entrega es una configuración y generación propias sobre el commit fijado en `seleccion.md`.

| Opción / archivo | Valor utilizado | Justificación |
|---|---|---|
| `conf.py` | Editado manualmente sobre la base generada con `sphinx-quickstart --sep sphinx` | Cumple el requisito de un `conf.py` propio; no se copió el de la documentación oficial de Rich. |
| `sys.path.insert(...)` | Ruta absoluta al checkout de `rich` clonado fuera del repositorio entregable | Necesario para que `autodoc` pueda importar el paquete `rich` en tiempo de construcción, ya que no se publicó como paquete instalado vía PyPI. |
| `extensions` | `sphinx.ext.autodoc`, `sphinx.ext.napoleon`, `sphinx.ext.viewcode`, `sphinx.ext.autosummary` | `autodoc` extrae documentación desde los módulos y *docstrings* reales del código; `napoleon` es necesaria porque los *docstrings* de `rich` (por ejemplo en `table.py`, `console.py`) usan estilo Google, no el formato reStructuredText nativo de Sphinx; `viewcode` genera los enlaces `[source]` hacia el código resaltado; `autosummary` genera tablas resumen automáticas por módulo. |
| `napoleon_google_docstring` | `True` | Se confirmó previamente, inspeccionando el código fuente, que los *docstrings* de Rich siguen el formato Google (secciones `Args:`, `Returns:`), no NumPy. |
| `napoleon_numpy_docstring` | `False` | Se desactiva explícitamente para evitar que Sphinx intente interpretar secciones con la sintaxis NumPy, que no se usa en este proyecto. |
| `autosummary_generate` | `True` | Activa la generación automática de páginas de resumen por módulo, cumpliendo el requisito de "`autosummary` o estrategia equivalente" junto con `sphinx-apidoc`. |
| `autodoc_default_options` | `members=True, undoc-members=False, show-inheritance=True` | Se documentan todos los miembros con *docstring* real; se optó por NO mostrar miembros sin documentar (`undoc-members=False`) para no llenar la referencia de entradas vacías, y se muestra la jerarquía de herencia entre clases como `Console`, `Table`, `Segment`. |
| `autodoc_mock_imports` | `[]` (vacío) | No fue necesario simular ninguna dependencia: `rich` solo requiere `pygments` y `markdown-it-py` como dependencias obligatorias, ambas instaladas en el entorno virtual. El único extra opcional (`ipywidgets`, para soporte de Jupyter) no se instaló ni se usó, y no bloqueó la importación del paquete. |
| `html_theme` | `sphinx_rtd_theme` | Se reemplazó el tema por defecto (`alabaster`) por el de Read the Docs, que ofrece navegación lateral con árbol expandible y mejor legibilidad para un paquete con más de 60 módulos. |
| `sphinx-apidoc -o sphinx/source/api rich/` | Generó `modules.rst` y `rich.rst` | Estrategia elegida para cubrir automáticamente los ~60 módulos del paquete sin escribir manualmente una directiva `automodule` por archivo. |
| `index.rst` | Página narrativa propia + `toctree` apuntando a `api/modules` | Se redactó manualmente el propósito del proyecto, su organización por módulos (`console.py`, `table.py`, `segment.py`, `style.py`, `progress.py`, etc.) y cómo navegar la referencia de API generada, cumpliendo el requisito de contenido narrativo escrito por la persona estudiante. |
| `requirements-docs.txt` | Generado con `pip freeze` dentro del entorno virtual `.venv-rich-docs` | Registra de forma reproducible únicamente las dependencias necesarias para construir la documentación (Sphinx, el tema, y las dependencias reales de `rich` para que `autodoc` pueda importarlo). |

Todas las rutas de `sys.path` y `sphinx-apidoc` apuntan al checkout local de `rich` fijado en el commit `9d8f9a372cc5916fd4781fec207ced7ddac2f08f`, clonado fuera del repositorio entregable; no se versiona el código fuente de `rich` en sí, solo la configuración de Sphinx, el registro de generación (`build.log`) y el HTML resultante en `site/python/`.


## 8.2 Consideraciones sobre autodoc

Dado que `autodoc` importa realmente el paquete `rich` para inspeccionar sus módulos, se tomaron las siguientes precauciones:

- **Entorno virtual exclusivo:** se creó `.venv-rich-docs` fuera del repositorio entregable, usado únicamente para construir esta documentación, sin mezclar dependencias con otros proyectos del sistema.
- **Dependencias instaladas:** se instalaron solo `sphinx`, `sphinx-rtd-theme`, y las dependencias declaradas como obligatorias por el propio proyecto en su `pyproject.toml`: `pygments` y `markdown-it-py`. No se instaló el extra opcional `ipywidgets` (usado solo para integración con Jupyter), ya que no es necesario para que `autodoc` importe el paquete correctamente. El procedimiento completo quedó registrado en `sphinx/requirements-docs.txt` mediante `pip freeze`.
- **Acceso al paquete desde `conf.py`:** se resolvió agregando la ruta absoluta del checkout de `rich` a `sys.path` mediante `sys.path.insert(0, os.path.abspath(...))`, ya que el paquete no se instaló como dependencia publicada sino que se documenta directamente desde el código fuente clonado.
- **`autodoc_mock_imports`:** se dejó vacío (`[]`) porque no fue necesario simular ninguna dependencia. Se verificó previamente, inspeccionando `rich/__init__.py`, que el paquete no ejecuta lógica de red, acceso a credenciales, ni otros efectos secundarios peligrosos al importarse; únicamente inicializa un objeto `Console` de forma perezosa (bajo demanda), no en tiempo de importación.
- **Revisión de advertencias de importación:** al ejecutar `sphinx-build`, no se registró ninguna advertencia del tipo `autodoc: failed to import module`, lo que confirma que las 62 páginas de módulos listadas en el log (`highlighting module code... rich.console`, `rich.table`, `rich.progress`, etc.) se generaron a partir de una importación real y exitosa del paquete, y no de una documentación parcial o simulada.
- **Confirmación de la API esperada:** se verificó manualmente que clases centrales del paquete, como `rich.table.Table` y `rich.console.Console`, aparecen documentadas en el HTML final con sus métodos, parámetros y el enlace `[source]` hacia el código resaltado por `viewcode`, confirmando que `autodoc` extrajo la API pública completa.


## 8.3 Advertencias obtenidas durante la generación

La generación finalizó exitosamente: `build succeeded, 15 warnings`, sin errores críticos que interrumpieran el proceso. Las 15 advertencias se agrupan en tres categorías, ninguna de las cuales afecta el contenido extraído de la API.

### Categoría 1 — Formato reStructuredText inválido dentro de los docstrings originales (7 advertencias)

```text
console.py: Console.render_lines — Block quote ends without a blank line
progress.py: Progress.get_default_columns — Definition list ends without a blank line (x2), Inline emphasis start-string without end-string
segment.py: Segment.align_bottom / align_middle / align_top / set_shape — Block quote ends without a blank line
```

Estas advertencias las emite `docutils`, el motor que interpreta reStructuredText dentro de Sphinx. Los *docstrings* de Rich fueron escritos priorizando que se lean bien como texto plano, pero no siguen estrictamente la sintaxis RST que `docutils` exige (por ejemplo, una lista o cita en bloque requiere una línea en blanco antes y después). No es un error de la configuración de este laboratorio, sino una discrepancia entre el estilo de escritura del autor original y el parser estricto de Sphinx; el contenido se sigue mostrando en el HTML, solo con un formato ligeramente distinto al que el autor probablemente pretendía.

### Categoría 2 — Descripciones duplicadas de objetos (3 advertencias)

```text
Segment.cell_length — duplicate object description
Tree.ASCII_GUIDES — duplicate object description
Tree.TREE_GUIDES — duplicate object description
```

Ocurre cuando un mismo símbolo (en estos casos, atributos de clase) queda documentado dos veces: una como atributo individual con su propio *docstring*, y otra al procesar la clase completa con `:members:`. Sphinx detecta que generaría dos entradas para el mismo símbolo en el índice de objetos y advierte, sugiriendo `:no-index:` para una de las dos. Es un patrón habitual al combinar `sphinx-apidoc` con clases que documentan atributos además de métodos.

### Categoría 3 — Referencias cruzadas a páginas fuera del alcance de esta build (4 advertencias)

```text
Console — undefined label 'console_markup'
Panel — undefined label 'appendix_box'
Table.Column — undefined label 'appendix_box'
Table.Table — undefined label 'appendix_box'
```

Los *docstrings* de Rich incluyen directivas `:ref:` que apuntan a secciones específicas de la documentación oficial completa del proyecto, publicada en Read the Docs (por ejemplo, un apéndice sobre estilos de caja o una guía de sintaxis de marcado). Como esta entrega es una configuración de Sphinx propia y mínima, y no se recreó la totalidad de las páginas narrativas adicionales del proyecto original, esas etiquetas de referencia no existen en este build y Sphinx no puede resolver el enlace. Esto afecta únicamente los hipervínculos cruzados hacia contenido fuera del alcance del laboratorio; no afecta las firmas, parámetros ni el resto del contenido extraído por `autodoc` en esas mismas clases.

### Por qué no se consideran críticas

Ninguna de las 15 advertencias impidió que `autodoc` extrajera correctamente las firmas, tipos y *docstrings* de la API pública del paquete. Todas son advertencias de presentación (formato RST en el texto original, objetos documentados en más de un lugar, o enlaces cruzados hacia páginas fuera del alcance de esta configuración), no de contenido faltante. El build generó exitosamente las 62 páginas de código fuente resaltado por `viewcode` y las páginas de referencia de todos los módulos del paquete.


### 8.4 Análisis de la documentación Sphinx

**1. Qué muestra la portada y cómo funciona el toctree**

La portada (https://ie0417-laboratorio-1-git-documentacio.netlify.app/python/) muestra la página narrativa escrita para este laboratorio: una introducción a Rich, la lista de repositorio/licencia/commit documentado, y una descripción de los módulos más relevantes (`console.py`, `table.py`, `segment.py`, `style.py`, `progress.py`, `markdown.py`, `syntax.py`). Al final de esa página, el `toctree` se renderiza como un enlace único hacia la sección **rich** (https://ie0417-laboratorio-1-git-documentacio.netlify.app/python/api/modules), que a su vez expande hacia **rich package** (https://ie0417-laboratorio-1-git-documentacio.netlify.app/python/api/rich). En la barra lateral del tema `sphinx_rtd_theme`, ese mismo árbol queda visible de forma permanente durante toda la navegación, no solo en la portada.

**2. Cómo se representan los paquetes, módulos, clases, funciones y métodos**

Toda la referencia de API vive en una sola página larga (https://ie0417-laboratorio-1-git-documentacio.netlify.app/python/api/rich), generada por `sphinx-apidoc`, organizada por **Submodules**: cada módulo del paquete (`rich.abc`, `rich.align`, `rich.console`, `rich.table`, etc.) aparece como una sección `## rich.<nombre> module`, y dentro de cada una, las clases se marcan con el prefijo *class* seguido del nombre completamente calificado (por ejemplo `class rich.console.Console`), y las funciones sueltas del módulo aparecen igual pero sin ese prefijo. Los métodos de cada clase quedan anidados debajo de su clase, cada uno con su propia firma completa.

**3. Qué contenido se obtuvo automáticamente de firmas y docstrings**

Todo el contenido técnico —firmas completas con *type hints*, la lista de parámetros con su tipo y descripción, valores de retorno, y las excepciones que puede lanzar cada método— proviene directamente de `autodoc` inspeccionando el código ya importado, sin que se haya escrito una sola línea de eso a mano. Por ejemplo, en la clase `Console` (https://ie0417-laboratorio-1-git-documentacio.netlify.app/python/api/rich#rich.console.Console), la firma del constructor lista automáticamente más de 25 parámetros con sus tipos (`color_system: Literal[...] | None`, `record: bool`, `width: int | None`, etc.), extraídos directamente de la definición real de `__init__` en `console.py`. La sección **Parameters** de cada método, con su descripción en prosa, viene del docstring de estilo Google interpretado por `napoleon` — por ejemplo el método `capture()` de `Console` (misma página, ancla `#rich.console.Console.capture`) muestra el tipo de retorno `Capture` y un ejemplo de uso completo tomado tal cual del docstring original.

**4. Qué contenido narrativo fue escrito manualmente y por qué era necesario**

La página `index.rst` completa (la portada) fue escrita manualmente para este laboratorio: la introducción al propósito de Rich, la tabla de datos del proyecto (repositorio, licencia, commit, versión) y la explicación de por qué la API se organiza por módulo y no por subpaquetes. Era necesaria porque `sphinx-apidoc` y `autodoc` solo saben generar referencia técnica a partir del código; no tienen forma de explicar el propósito general del proyecto ni orientar a alguien que llega por primera vez sin ya saber qué es Rich.

**5. Cómo se presentan parámetros, tipos, retornos, excepciones, índices, búsqueda y enlaces al código**

Cada método muestra bloques etiquetados **Parameters**, **Returns**, **Return type**, y cuando aplica, **Raises** — por ejemplo, `Align.__init__` (https://ie0417-laboratorio-1-git-documentacio.netlify.app/python/api/rich#rich.align.Align) documenta explícitamente `Raises: ValueError – if align is not one of the expected values`. El índice general y el módulo (`genindex`, `py-modindex`) y el buscador quedaron generados automáticamente por Sphinx y confirmamos manualmente que el buscador devuelve resultados reales (por ejemplo, "Table"). Cada símbolo tiene un enlace `[source]` gracias a `viewcode` que lleva directamente al código resaltado — por ejemplo, el de la clase `Console` apunta a https://ie0417-laboratorio-1-git-documentacio.netlify.app/python/_modules/rich/console#Console.

**6. Qué puede aprender una persona usuaria o desarrolladora nueva sobre el proyecto**

Alguien nuevo puede entender, solo con la portada, que Rich es una librería de formateo de terminal organizada por responsabilidad (una clase por tipo de renderizado), y luego profundizar directamente en la clase que necesite —por ejemplo `Table` o `Progress`— sin tener que leer el código fuente primero, ya que cada clase documenta sus parámetros de construcción con ejemplos de uso reales incluidos en el propio docstring (como el ejemplo de `Align` con `Console`, `Align` y `Panel` combinados).

**7. Qué elementos quedaron incompletos, poco claros o sin documentar, y por qué**

Como se detalló en la sección 8.3, quedaron 15 advertencias: inconsistencias de formato reStructuredText en algunos docstrings (`console.py`, `progress.py`, `segment.py`), tres símbolos con descripción duplicada (`Segment.cell_length`, `Tree.ASCII_GUIDES`, `Tree.TREE_GUIDES`), y cuatro referencias cruzadas (`:ref:`) que no se resuelven porque apuntan a páginas de la documentación oficial completa de Rich (por ejemplo `appendix_box`), no incluidas en esta configuración mínima. Ninguna de ellas oculta contenido de la API; solo afectan formato o enlaces cruzados secundarios.

**Enlaces representativos utilizados en este análisis:**
1. https://ie0417-laboratorio-1-git-documentacio.netlify.app/python/
2. https://ie0417-laboratorio-1-git-documentacio.netlify.app/python/api/modules
3. https://ie0417-laboratorio-1-git-documentacio.netlify.app/python/api/rich#rich.console.Console
4. https://ie0417-laboratorio-1-git-documentacio.netlify.app/python/api/rich#rich.align.Align
5. https://ie0417-laboratorio-1-git-documentacio.netlify.app/python/_modules/rich/console#Console



## 9. Parte V - Comparación entre Doxygen y Sphinx

### Tabla comparativa

| Dimensión | Doxygen en C++ (LevelDB) | Sphinx en Python (Rich) |
|---|---|---|
| **Fuente principal de la información** | El código fuente mismo: declaraciones en headers (`.h`), definiciones en `.cc`, y comentarios `//` de línea ubicados junto a cada símbolo. Doxygen analiza sintácticamente el C++ sin ejecutarlo. | Los *docstrings* reales de los objetos en memoria: Sphinx (vía `autodoc`) **importa y ejecuta** el paquete Python, y extrae `__doc__` de cada módulo, clase y función ya cargada, no el texto crudo del archivo. |
| **Configuración y proceso de generación** | Un único archivo `Doxyfile` de texto plano con cientos de opciones; un solo comando (`doxygen Doxyfile`) ejecuta todo el análisis y la generación HTML sin pasos intermedios. | Varios artefactos coordinados: `conf.py` (configuración), `sphinx-apidoc` (genera `.rst` por módulo) y `sphinx-build` (compila a HTML); además requiere un entorno virtual con las dependencias reales del proyecto instaladas para que la importación no falle. |
| **Organización y navegación** | Navegación por *Namespaces*, *Classes*, *Files*, *Class Hierarchy*, todas generadas automáticamente a partir de la estructura del código; no hay una portada narrativa a menos que se provea un `mainpage.md` explícito (como se hizo aquí). | Navegación gobernada por el `toctree` que la persona autora escribe a mano en `index.rst`; la portada es obligatoriamente narrativa desde el inicio, y la referencia de API (`api/modules`) se integra como una rama más de ese árbol, no como una vista separada. |
| **Documentación de API** | Por archivo y por símbolo: cada `.h`/`.cc` tiene su propia página, y cada clase muestra sus miembros con la firma completa de C++ (tipos, `const`, referencias, punteros). | Por módulo: `sphinx-apidoc` genera una entrada `automodule` por archivo `.py`, y dentro de cada una se listan las clases y funciones con sus firmas de Python (incluyendo *type hints* cuando existen) y el cuerpo del docstring interpretado como texto enriquecido. |
| **Diagramas y referencias cruzadas** | Fuertes y automáticos vía Graphviz: diagramas de herencia (`CLASS_GRAPH`), de colaboración (`COLLABORATION_GRAPH`) y de llamadas (`CALL_GRAPH`/`CALLER_GRAPH`) generados sin que el autor escriba nada adicional, más allá de activarlos en el `Doxyfile`. | Prácticamente inexistentes por defecto: Sphinx no genera diagramas de llamadas ni de colaboración de fábrica; solo ofrece referencias cruzadas textuales (enlaces `:ref:`, `:class:`, y los enlaces `[source]` de `viewcode`). Obtener diagramas equivalentes requeriría una extensión adicional (por ejemplo `sphinx.ext.inheritance_diagram`, no usada en esta entrega). |
| **Contenido narrativo** | Opcional y externo al flujo normal: por defecto Doxygen no pide una portada narrativa; solo se logró una página de inicio con contenido real porque se creó manualmente `mainpage.md` y se vinculó con `USE_MDFILE_AS_MAINPAGE`. | Obligatorio y estructural: `index.rst` es el punto de entrada de cualquier proyecto Sphinx, por lo que la narrativa (propósito, organización del proyecto) es parte inseparable de la configuración mínima, no un añadido. |
| **Dependencia de comentarios o *docstrings*** | Tolerante a comentarios informales: gracias a `JAVADOC_AUTOBRIEF`, Doxygen aprovechó comentarios `//` simples que ni siquiera fueron escritos pensando en Doxygen. Sin esa opción, la documentación de texto habría salido vacía. | Estricta con el formato: `napoleon` necesita que los *docstrings* seân razonablemente consistentes en su estructura (secciones `Args:`/`Returns:` de estilo Google); cuando el *docstring* usa una sintaxis RST inconsistente (como ocurrió en `progress.py` o `segment.py`), Sphinx/`docutils` lo señala explícitamente como advertencia de formato. |
| **Facilidad de mantenimiento** | Bajo mantenimiento activo del *Doxyfile* una vez configurado: agregar un nuevo archivo `.cc` a una carpeta ya incluida en `INPUT` no requiere tocar la configuración. | Requiere un paso adicional: si se agrega un nuevo módulo `.py` al paquete, hay que volver a correr `sphinx-apidoc` (o mantener `autosummary_generate` funcionando) para que la referencia lo incluya; el `conf.py` en sí rara vez cambia, pero el árbol de `.rst` generado sí necesita regenerarse. |
| **Audiencia principal** | Desarrolladores que van a **modificar o depurar** LevelDB internamente: los grafos de llamadas y colaboración son más útiles para entender flujo interno que para simplemente "usar" la librería desde afuera. | Tanto desarrolladores como usuarios finales de la librería: la portada narrativa y la referencia de API en un mismo sitio están pensadas para alguien que solo quiere **usar** `rich.table.Table` sin necesariamente leer su implementación interna. |
| **Fortalezas y limitaciones** | *Fortaleza:* diagramas ricos sin esfuerzo adicional, tolerancia a comentarios no estandarizados. *Limitación:* la narrativa de alto nivel es responsabilidad externa del autor; sin ella, la documentación es solo referencia técnica fría. | *Fortaleza:* integración natural entre narrativa y referencia, tema legible por defecto, ecosistema más amigable para publicar (Read the Docs, GitHub Pages). *Limitación:* no genera diagramas estructurales por sí sola, y es más sensible a inconsistencias de formato en los *docstrings* existentes. |

### 1. ¿Cuál herramienta produjo información útil con menos configuración y por qué?

**Doxygen.** Un solo archivo (`Doxyfile`) y un solo comando (`doxygen Doxyfile`) fueron suficientes para obtener una documentación con navegación completa, firmas de todas las clases y diagramas de colaboración, incluso sin que LevelDB tuviera comentarios en formato Doxygen "oficial" — bastó con activar `JAVADOC_AUTOBRIEF` para aprovechar los comentarios `//` ya existentes. Sphinx, en cambio, necesitó un entorno virtual, instalar las dependencias reales del proyecto (`pygments`, `markdown-it-py`), resolver el `sys.path` para que `autodoc` pudiera importar el paquete, y correr `sphinx-apidoc` como paso separado antes de poder generar el HTML. La diferencia de fondo es que Doxygen solo necesita *leer* el código, mientras que Sphinx/`autodoc` necesita poder *ejecutarlo* (importarlo), lo cual añade una capa completa de gestión de entorno que Doxygen no requiere.

### 2. ¿Cuál resultado ayuda mejor a comprender la arquitectura del proyecto?

**Doxygen sobre LevelDB.** Los diagramas de colaboración y de llamadas muestran de forma visual e inmediata cómo `DBImpl` se apoya en `VersionSet`, `MemTable` y `TableCache`, sin que el lector tenga que leer código para inferir esas relaciones. Sphinx, sin una extensión de diagramas activada, deja esa tarea completamente en manos de la narrativa escrita a mano en `index.rst`: en esta entrega, la comprensión de cómo se relacionan `Console`, `Table` y `Segment` depende de que la persona autora lo haya explicado bien en prosa, no de una visualización generada automáticamente del código real.

### 3. ¿Cuál resultado ayuda mejor a aprender a utilizar la API?

**Sphinx sobre Rich.** Al integrar la narrativa introductoria y la referencia de API en el mismo sitio de navegación (`toctree`), y al interpretar los *docstrings* de estilo Google con `napoleon`, cada método de `Table` o `Console` muestra sus parámetros, tipos y comportamiento en un formato legible pensado para alguien que solo quiere consumir la librería. Doxygen sobre LevelDB, en cambio, produce documentación más orientada a quien va a leer o modificar la implementación interna: es más común encontrar comentarios explicando *cómo* funciona un método por dentro que documentación pensada explícitamente para el consumidor externo de la API pública.

### 4. ¿Qué problemas del código fuente quedaron expuestos al generar la documentación?

- En **LevelDB**, quedaron expuestas tres funciones/símbolos (`PosixEnv::PosixEnv`, `PosixEnv::BackgroundThreadMain`, `FileState::DropUnsyncedData`) cuya documentación en el `.cc` no pudo enlazarse con una declaración procesada por Doxygen, y una firma (`PosixEnv::Schedule`) cuyos parámetros no coinciden exactamente entre la declaración y la definición. Esto sugiere inconsistencias menores entre cómo se documentó el código internamente y cómo está realmente declarado, específicas de la implementación POSIX.
- En **Rich**, quedaron expuestas inconsistencias de formato reStructuredText en varios *docstrings* (`console.py`, `progress.py`, `segment.py`) — bloques de cita y listas de definición sin la línea en blanco que RST exige — y tres símbolos documentados en más de un lugar (`Segment.cell_length`, `Tree.ASCII_GUIDES`, `Tree.TREE_GUIDES`), generando advertencias de "descripción duplicada". También quedó expuesto que varios *docstrings* referencian secciones (`:ref:`) que solo existen en la documentación oficial completa del proyecto, no en el código en sí, revelando una dependencia implícita entre el código fuente y páginas narrativas externas que no viajan junto con el propio paquete.

### 5. ¿Qué cambios integraría al flujo de desarrollo para mantener la documentación actualizada?

- Adoptar un estilo de comentario consistente y explícito desde el inicio del proyecto (comentarios `///` o `/** */` en C++, *docstrings* de estilo Google verificados con una herramienta de *linting* en Python), en lugar de depender de que Doxygen "adivine" con `JAVADOC_AUTOBRIEF` o de que `napoleon` tolere variaciones de formato.
- Regenerar la documentación como parte del flujo de *pull requests*, no solo antes de un release: cualquier cambio que agregue un nuevo archivo, clase o módulo debería disparar una nueva ejecución de `doxygen`/`sphinx-apidoc` para detectar de inmediato si el símbolo nuevo quedó sin documentar.
- Versionar el `Doxyfile` y el `conf.py`/`requirements-docs.txt` junto con el código (como se hizo en este laboratorio), para que la configuración de documentación evolucione con el proyecto y no se desactualice silenciosamente.
- Corregir de forma proactiva las advertencias de formato (RST o de emparejamiento de firmas) en lugar de acumularlas, ya que aunque no son errores críticos, degradan progresivamente la calidad de la documentación generada.

### 6. ¿Qué verificaciones automatizaría en integración continua?

- **Fallar el build si aparecen errores críticos** de Doxygen o Sphinx (no advertencias, sino errores de parseo reales), para evitar publicar documentación rota.
- **Contar y limitar el número de advertencias permitidas**: por ejemplo, fallar el pipeline si el número de advertencias de Doxygen o Sphinx crece respecto al build anterior, evitando que la deuda de documentación se acumule silenciosamente.
- **Verificar enlaces internos rotos**, usando herramientas como `linkchecker` sobre el HTML generado, o el propio builder `linkcheck` de Sphinx (`sphinx-build -b linkcheck`), para detectar referencias `:ref:` no resueltas como las que aparecieron con `appendix_box` y `console_markup`.
- **Comprobar que `autodoc` efectivamente importa el paquete sin errores** en un entorno limpio (recreando el entorno virtual desde `requirements-docs.txt` en cada corrida de CI), para detectar cuanto antes si un cambio en las dependencias del proyecto rompe la generación de documentación.
- **Verificar cobertura de documentación**: por ejemplo, con `interrogate` o `docstr-coverage` para Python, o revisando el reporte de símbolos no documentados que puede producir Doxygen, para alertar cuando el porcentaje de símbolos públicos sin documentar supere un umbral definido por el equipo.
- **Publicar automáticamente la documentación generada** (por ejemplo a Netlify o GitHub Pages) solo si todas las verificaciones anteriores pasan, asegurando que el sitio público nunca refleje una build fallida o con advertencias no revisadas.

## 10.3 Verificación obligatoria

La verificación de las tres páginas publicadas se realizó el **30/08/2026 a las 20:20**, utilizando las siguientes URL:

- **Página principal:** https://ie0417-laboratorio-1-git-documentacio.netlify.app/
- **Documentación C++:** https://ie0417-laboratorio-1-git-documentacio.netlify.app/cpp/
- **Documentación Python:** https://ie0417-laboratorio-1-git-documentacio.netlify.app/python/

Durante la prueba se comprobó que:

1. Las tres URL son accesibles sin necesidad de autenticación.
2. La portada y las rutas `/cpp/` y `/python/` cargan correctamente.
3. La navegación por clases, módulos, archivos y páginas internas funciona correctamente.
4. Los recursos de la documentación, incluyendo CSS, JavaScript, imágenes, diagramas y búsqueda, se cargan y funcionan correctamente.
5. Los enlaces internos de las páginas fueron verificados y funcionan correctamente.
6. Las URL se encuentran publicadas y disponibles para su revisión.

## 11. Uso de Git y GitHub en la propia entrega

### Estrategia de ramas e integración

El desarrollo de las partes documentales del laboratorio (análisis de Git,
informe técnico, README y el análisis final de las páginas publicadas) se
realizó en ramas `feature/*` separadas, cada una dedicada a un bloque de
trabajo coherente y verificable de forma independiente. La integración a
`main` se hizo mediante `git merge --no-ff` en cada caso, en lugar de
`rebase` o *fast-forward*, para preservar explícitamente en el historial
en qué rama se desarrolló cada bloque de trabajo. Esto se consideró más
informativo para efectos de este laboratorio, ya que permite visualizar
con `git log --graph` la topología completa de ramas y su punto de
integración, en lugar de aplanar el historial a una sola línea recta.

`main` se mantuvo como rama estable: solo recibió código mediante merges
desde ramas de trabajo ya completas y verificadas, no commits parciales
o experimentales.

### Salida de `git log --graph --oneline --decorate --all`

```text
*   d6cbc28 (HEAD -> main, origin/main) Merge branch 'fix/informe-gitlog' into main
|\
| * 3d9b4c7 (origin/fix/informe-gitlog, fix/informe-gitlog) docs(informe): agregar seccion 11 con salida de git log y estrategia de integracion
|/
*   2aa796e Merge branch 'feature/informe' into main
|\
| * 5d515b4 (origin/feature/informe, feature/informe) docs(informe): Documentación completa del informe
|/
*   51c18bb Merge branch 'feature/readme' into main
|\
| * 487e75c (origin/feature/readme, feature/readme) docs: agregar README con instrucciones de instalacion y estructura del proyecto
|/
*   3abe28d Merge branch 'feature/analisis-git' into main
|\
| * 4889932 (origin/feature/analisis-git, feature/analisis-git) docs(git): agregar analisis obligatorio de Git (5.3) y evidencias de Learn Git Branching
|/
* 9a3f294 fix: corregir estructura de site/cpp para que index.html quede en la raiz de /cpp/
* ae73672 Publicar sitio estatico: configuracion y HTML de Doxygen (LevelDB) y Sphinx (Rich)
```


### Tag de entrega

Se creó el tag anotado `v1.0-laboratorio` sobre el commit final de la
rama `main`, marcando el estado entregado para revisión:

```bash
git tag -a v1.0-laboratorio -m "Entrega final Laboratorio 1 - IE0417"
```