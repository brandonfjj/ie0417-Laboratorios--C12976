.. Rich documentation master file, created by
   sphinx-quickstart on Sat Aug 29 21:33:04 2026.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Documentación de Rich
======================

Esta documentación fue generada como parte del Laboratorio 1 (IE0417) usando
Sphinx sobre el código fuente de **Rich**, una librería de Python para
generar salida de texto enriquecida y con formato en la terminal: tablas,
barras de progreso, resaltado de sintaxis, renderizado de Markdown, árboles,
y *tracebacks* mejorados, entre otras utilidades.

Sobre el proyecto original
---------------------------

- **Repositorio original:** https://github.com/Textualize/rich
- **Licencia:** MIT
- **Commit documentado:** ``9d8f9a372cc5916fd4781fec207ced7ddac2f08f``
- **Versión:** 15.0.0

Rich está organizado como un único paquete Python (``rich/``) compuesto por
módulos independientes, cada uno responsable de un tipo de renderizado o de
una utilidad interna. Entre los módulos más relevantes para entender la
arquitectura del proyecto están:

- ``console.py``: implementa la clase ``Console``, el punto de entrada
  principal que coordina cómo se escribe cualquier contenido enriquecido
  hacia la terminal.
- ``table.py``: define ``Table`` y ``Column``, usadas para construir tablas
  con alineación, estilos y bordes configurables.
- ``segment.py``: representa la unidad mínima de texto con estilo (un
  "segmento") sobre la que se construyen todos los demás renderizadores.
- ``style.py``: maneja la representación de estilos de texto (color,
  negrita, subrayado, etc.) aplicados a los segmentos.
- ``progress.py``: implementa las barras de progreso y sus columnas
  configurables.
- ``markdown.py`` y ``syntax.py``: convierten Markdown y código fuente,
  respectivamente, en salida coloreada para la terminal.

Esta separación por responsabilidad es la razón por la que la referencia de
API generada automáticamente a continuación agrupa la documentación por
módulo: cada archivo ``.py`` del paquete corresponde a una responsabilidad
concreta dentro de la librería, y no existe una jerarquía de subpaquetes
profunda que documentar por separado.

Cómo navegar esta documentación
---------------------------------

La sección **API** (generada con ``sphinx-apidoc`` y ``autodoc``) contiene la
referencia completa de clases, funciones y métodos extraída directamente de
las firmas y *docstrings* del código fuente. Esta página narrativa solo
cubre el propósito y la organización general; los detalles de cada símbolo
público —parámetros, tipos, valores de retorno— se encuentran en el árbol de
módulos enlazado abajo.

.. toctree::
   :maxdepth: 2
   :caption: Contenidos:

   api/modules